// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"

#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "SCharacter.h"
#include "TimerManager.h"
#include "AI/SAICharacter.h"
#include "Algo/AllOf.h"
#include "Components/SAttributeComponent.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "Logging/StructuredLog.h"
#include "Player/SPlayerState.h"


namespace SConsoleVariables
{
	TAutoConsoleVariable CVarSpawnBots(
        TEXT("su.SpawnBots"),
        true,
        TEXT("Enable bot spawning"),
        ECVF_Cheat);
}

ASGameModeBase::ASGameModeBase()
{
	PlayerStateClass = ASPlayerState::StaticClass();
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();

	if (MinionBotClass)
	{
		GetWorldTimerManager()
			.SetTimer(
				TimerHandle_SpawnBots,
				this,
				&ASGameModeBase::SpawnBotTimerElapsed,
				SpawnTimeInterval,
				true);
	}

	if (!PickUpItemClasses.IsEmpty() && ensure(PickUpItemSpawnQuery))
	{
		auto* EQSQueryInstance = UEnvQueryManager::RunEQSQuery(
			this,
			PickUpItemSpawnQuery,
			this,
			EEnvQueryRunMode::AllMatching,
			nullptr);

		EQSQueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnPickUpItemSpawnQueryCompleted);
	}
}

void ASGameModeBase::OnActorKilled(AActor* VictimActor, AActor* KillerActor)
{
	if (const auto* PlayerCharacter = Cast<ASCharacter>(VictimActor); IsValid(PlayerCharacter))
	{
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUFunction(this, TEXT("RespawnPlayerElapsed"), PlayerCharacter->GetController());

		FTimerHandle TimerHandle_RespondDelay;
		constexpr auto RespawnDelay = 2.f;
		GetWorldTimerManager().SetTimer(TimerHandle_RespondDelay, TimerDelegate, RespawnDelay, false);
	}

	if (const auto* KillerPawn = Cast<APawn>(KillerActor); IsValid(KillerPawn))
	{
		if (auto* PlayerState = KillerPawn->GetPlayerState<ASPlayerState>(); IsValid(PlayerState))
		{
			PlayerState->AddCredits(CreditsPerKill);
		}
	}
}

void ASGameModeBase::RespawnPlayerElapsed(AController* Controller)
{
	if (ensure(IsValid(Controller)))
	{
		Controller->UnPossess();

		RestartPlayer(Controller);
	}
}

void ASGameModeBase::KillAll()
{
	for (TActorIterator<ASAICharacter> It{GetWorld()}; It; ++It)
	{
		auto* AttributeComponent = USAttributeComponent::GetAttributeComponent(*It);
		if (ensure(IsValid(AttributeComponent)) && AttributeComponent->IsAlive())
		{
			AttributeComponent->Kill(this);
		}
	}
}

void ASGameModeBase::SpawnBotTimerElapsed()
{
	if (!SConsoleVariables::CVarSpawnBots.GetValueOnGameThread())
	{
		UE_LOGFMT(LogTemp, Warning, "Bot spawning is disabled via cvar 'CVarSpawnBots'.");
		return;
	}

	int32 AliveBotsNum = 0;

	for (TActorIterator<ASAICharacter> It{GetWorld()}; It; ++It)
	{
		const auto* AttributeComponent = USAttributeComponent::GetAttributeComponent(*It);
		if (ensure(IsValid(AttributeComponent)) && AttributeComponent->IsAlive())
		{
			++AliveBotsNum;
		}
	}

	int32 AliveBotsMaxCount = 0;

	if (DifficultyCurve)
	{
		AliveBotsMaxCount = static_cast<int32>(DifficultyCurve->GetFloatValue(GetWorld()->TimeSeconds));
	}

	UE_LOGFMT(LogTemp, Log, "Found {0} alive bots", AliveBotsNum);

	if (AliveBotsNum >= AliveBotsMaxCount)
	{
		UE_LOGFMT(LogTemp, Log, "Maximum bot capacity has reached.");
		return;
	}

	auto* QueryInstance = UEnvQueryManager::RunEQSQuery(
		this,
		FindBotSpawnQuery,
		this,
		EEnvQueryRunMode::RandomBest5Pct,
		nullptr);

	if (ensure(IsValid(QueryInstance)))
	{
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnFindBotSpawnQueryCompleted);
	}
}

void ASGameModeBase::OnFindBotSpawnQueryCompleted(
	UEnvQueryInstanceBlueprintWrapper* QueryInstance,
	EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOGFMT(LogTemp, Warning, "Spawn bot EQS query failed");
		return;
	}

	auto Locations = QueryInstance->GetResultsAsLocations();
	if (Locations.IsEmpty())
	{
		return;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	auto* SpawnedBot = GetWorld()->SpawnActor<AActor>(MinionBotClass, Locations[0], FRotator::ZeroRotator, SpawnParameters);
	if (ensure(IsValid(SpawnedBot)))
	{
		DrawDebugSphere(GetWorld(), SpawnedBot->GetActorLocation(), 50, 20, FColor::Blue, false, 60);
	}
}

void ASGameModeBase::OnPickUpItemSpawnQueryCompleted(
	UEnvQueryInstanceBlueprintWrapper* QueryInstance,
	EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOGFMT(LogTemp, Warning, "Spawn pick up item EQS query failed");
		return;
	}

	TArray<FVector> QueryResults;
	QueryInstance->GetQueryResultsAsLocations(QueryResults);

	if (QueryResults.IsEmpty())
	{
		return;
	}

	int32 SpawnCounter = 0;
	int32 EndIndex = QueryResults.Num();

	const float RequiredPickUpItemsDistanceSquared = FMath::Square(RequiredPickUpItemsDistance);

	while (SpawnCounter < DesiredPickUpItemsCount && EndIndex > 0)
	{
		{
			const int32 RandomIndex = FMath::RandRange(0, --EndIndex);
			if (!ensure(QueryResults.IsValidIndex(RandomIndex)))
			{
				break;
			}

			if (RandomIndex != EndIndex)
			{
				QueryResults.Swap(RandomIndex, EndIndex);
			}
		}

		const auto& SpawnLocation = QueryResults[EndIndex];

		const TConstArrayView<FVector> PickedSpawnLocations{
			QueryResults.GetData() + EndIndex + 1,
			QueryResults.Num() - EndIndex - 1};

		const auto bValidLocation = PickedSpawnLocations.IsEmpty() || Algo::AllOf(
			PickedSpawnLocations,
			[&SpawnLocation, RequiredPickUpItemsDistanceSquared](const FVector& Location)
			{
				const auto DistSquared = FVector::DistSquared(SpawnLocation, Location);
				return DistSquared >= RequiredPickUpItemsDistanceSquared;
			});

		if (!bValidLocation)
		{
			continue;
		}
	
		const int32 RandomPickUpItemIndex = FMath::RandRange(0, PickUpItemClasses.Num() - 1);
		if (!ensure(PickUpItemClasses.IsValidIndex(RandomPickUpItemIndex)))
		{
			break;
		}

		const auto& PickUpItemClass = PickUpItemClasses[RandomPickUpItemIndex];
		if (!ensure(PickUpItemClass))
        {
            break;
        }

		GetWorld()->SpawnActor<AActor>(PickUpItemClass, SpawnLocation, FRotator::ZeroRotator);

		++SpawnCounter;
	}
}
