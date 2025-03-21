// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"

#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "SCharacter.h"
#include "SSaveGame.h"
#include "TimerManager.h"
#include "ActionRogueLike/ActionRogueLike.h"
#include "AI/SAICharacter.h"
#include "Algo/AllOf.h"
#include "AssetsData/SMonsterData.h"
#include "Components/SActionComponent.h"
#include "Components/SAttributeComponent.h"
#include "Engine/AssetManager.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"
#include "Player/SPlayerState.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"


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

	if (ensure(IsValid(MonsterDataTable)))
	{
		GetWorldTimerManager()
			.SetTimer(
				TimerHandle_SpawnBots,
				this,
				&ASGameModeBase::SpawnBotTimerElapsed,
				SpawnTimeInterval,
				true);
	}
	else
	{
		LogOnScreen(this, TEXT("Invalid monsters table reference"), FColor::Red);
	}

	if (!PickUpItemClasses.IsEmpty() && ensure(PickUpItemSpawnQuery))
	{
		FEnvQueryRequest Request{PickUpItemSpawnQuery, nullptr};
		Request.Execute(EEnvQueryRunMode::AllMatching, this, &ThisClass::OnPickUpItemSpawnQueryCompleted);
	}
}

void ASGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	if (auto* PlayerState = NewPlayer->GetPlayerState<ASPlayerState>(); IsValid(PlayerState))
	{
		PlayerState->LoadState(CurrentSaveGame);
	}

	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
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

		FSActorSaveData ActorSaveData{
			.ActorName = It->GetName(),
			.Transform = It->GetActorTransform()
		};

		{
			FMemoryWriter Writer{ActorSaveData.ByteData};
			FObjectAndNameAsStringProxyArchive Archive{Writer, true};
			Archive.ArIsSaveGame = true;

			It->Serialize(Archive);
		}

		CurrentSaveGame->SavedActors.Emplace(ActorSaveData);
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

			const auto* ActorSaveData = CurrentSaveGame->SavedActors.FindByPredicate(
				[It](const FSActorSaveData& Data)
				{
					return Data.ActorName == It->GetName();
				});

			if (ActorSaveData != nullptr)
			{
				It->SetActorTransform(ActorSaveData->Transform);

				{
					FMemoryReader Reader{ActorSaveData->ByteData};
					FObjectAndNameAsStringProxyArchive Archive{Reader, true};
					Archive.ArIsSaveGame = true;

					It->Serialize(Archive);
				}

				ISGameplayInterface::Execute_OnActorLoaded(*It);
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

	FEnvQueryRequest Request{FindBotSpawnQuery, nullptr};
	const auto QueryID = Request.Execute(EEnvQueryRunMode::RandomBest5Pct, this, &ThisClass::OnFindBotSpawnQueryCompleted);
	if (QueryID == INDEX_NONE)
	{
		LogOnScreen(
			this,
			FString::Printf(TEXT("Failed to execute EQS query")),
			FColor::Red);
		UE_LOGFMT(LogTemp, Warning, "Failed to execute EQS query");
	}
}

void ASGameModeBase::OnFindBotSpawnQueryCompleted(TSharedPtr<FEnvQueryResult> QueryResult)
{
	const auto* QueryResultPrr = QueryResult.Get();
	if (QueryResultPrr == nullptr || !QueryResultPrr->IsSuccessful())
	{
		UE_LOGFMT(LogTemp, Warning, "Spawn bot EQS query failed");
		return;
	}

	TArray<FVector> Locations;
	QueryResultPrr->GetAllAsLocations(Locations);

	if (Locations.IsEmpty())
	{
		return;
	}

	if (!ensure(IsValid(MonsterDataTable)))
	{
		return;
	}

	TArray<FSMonsterInfoEntry*> Monsters;
	MonsterDataTable->GetAllRows("", Monsters);

	const int32 Index = FMath::RandRange(0, Monsters.Num() - 1);
	if (!ensure(Monsters.IsValidIndex(Index)))
	{
		return;
	}

	const auto MonsterAssetId = Monsters[Index]->MonsterAssetId;
	if (!ensure(MonsterAssetId.IsValid()))
	{
		return;
	}

	auto& AssetManager = UAssetManager::Get();

	TArray<FName> Bundles;
	AssetManager.LoadPrimaryAsset(
		MonsterAssetId,
		Bundles,
		FStreamableDelegate::CreateUObject(
			this,
			&ThisClass::OnMonsterAssetLoaded,
			MonsterAssetId,
			Locations[0]));
}

void ASGameModeBase::OnPickUpItemSpawnQueryCompleted(TSharedPtr<FEnvQueryResult> QueryResult)
{
	const auto* QueryResultPrr = QueryResult.Get();
	if (QueryResultPrr == nullptr || !QueryResultPrr->IsSuccessful())
	{
		UE_LOGFMT(LogTemp, Warning, "Spawn pick up item EQS query failed");
		return;
	}

	TArray<FVector> QueryResults;
	QueryResultPrr->GetAllAsLocations(QueryResults);

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

void ASGameModeBase::OnMonsterAssetLoaded(FPrimaryAssetId PrimaryAssetId, FVector SpawnLocation)
{
	LogOnScreen(this, TEXT("Finished loading monster asset"), FColor::Green);

	const auto& AssetManager = UAssetManager::Get();

	auto* MonsterData = Cast<USMonsterData>(AssetManager.GetPrimaryAssetObject(PrimaryAssetId));
	if (!ensure(IsValid(MonsterData)))
	{
		return;
	}

	const auto MonsterClass = MonsterData->MonsterClass;
	if (!ensure(IsValid(MonsterClass)))
	{
		return;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	auto* SpawnedBot = GetWorld()->SpawnActor<AActor>(MonsterClass, SpawnLocation, FRotator::ZeroRotator, SpawnParameters);
	if (!(ensure(IsValid(SpawnedBot))))
	{
		return;
	}

	if (auto* ActionComponent = SpawnedBot->GetComponentByClass<USActionComponent>(); IsValid(ActionComponent))
	{
		for (const auto GrantedAction : MonsterData->GrantedActions)
		{
			ActionComponent->AddAction(this, GrantedAction);
		} 
	}
}
