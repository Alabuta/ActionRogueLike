// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"

#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "SCharacter.h"
#include "SSaveGame.h"
#include "TimerManager.h"
#include "AI/SAICharacter.h"
#include "Algo/AllOf.h"
#include "Components/SAttributeComponent.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
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

void ASGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	LoadSaveGame();
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

void ASGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	if (auto* PlayerState = NewPlayer->GetPlayerState<ASPlayerState>(); IsValid(PlayerState))
	{
		PlayerState->LoadState(CurrentSaveGame);
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

void ASGameModeBase::WriteSaveGame()
{
	for (APlayerState* PlayerState : GameState->PlayerArray)
	{
		if (auto* PS = Cast<ASPlayerState>(PlayerState); IsValid(PS))
		{
			PS->SaveState(CurrentSaveGame);
			break; // Save only first player state
		}
	}

	CurrentSaveGame->SavedActors.Reset();

	for (FActorIterator It{GetWorld()}; It; ++It)
	{
		if (!It->Implements<USGameplayInterface>())
		{
			continue;
		}

		CurrentSaveGame->SavedActors.Emplace(
			FSActorSaveData{
				.ActorName = It->GetName(),
				.Transform = It->GetActorTransform()
			});
	}
	
	UGameplayStatics::SaveGameToSlot(CurrentSaveGame, SlotName, 0);
}

void ASGameModeBase::LoadSaveGame()
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
		if (!IsValid(CurrentSaveGame))
		{
			UE_LOGFMT(LogTemp, Error, "Failed to load save game from slot {0}", *SlotName);
			return;
		}

		UE_LOGFMT(LogTemp, Log, "Loaded save game from slot {0}", *SlotName);

		for (FActorIterator It{GetWorld()}; It; ++It)
		{
			if (!It->Implements<USGameplayInterface>())
			{
				continue;
			}

			const auto* SavedData = CurrentSaveGame->SavedActors.FindByPredicate(
				[It](const FSActorSaveData& Data)
				{
					return Data.ActorName == It->GetName();
				});

			if (SavedData != nullptr)
			{
				It->SetActorTransform(SavedData->Transform);
			}
		}
	}
	else
	{
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::CreateSaveGameObject(USSaveGame::StaticClass()));
		if (IsValid(CurrentSaveGame))
		{
			UE_LOGFMT(LogTemp, Log, "Created new save game object");
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
