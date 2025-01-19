// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"

#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "TimerManager.h"
#include "AI/SAICharacter.h"
#include "Components/SAttributeComponent.h"
#include "EnvironmentQuery/EnvQueryManager.h"

ASGameModeBase::ASGameModeBase()
{
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();

	GetWorldTimerManager()
		.SetTimer(
			TimerHandle_SpawnBots,
			this,
			&ASGameModeBase::SpawnBot,
			SpawnTimeInterval,
			true);
}

void ASGameModeBase::SpawnBot()
{
	int32 AliveBotsNum = 0;
	for (TActorIterator<ASAICharacter> It{GetWorld()}; It; ++It)
	{
		const auto* AttributeComponent = USAttributeComponent::GetAttributeComponent(*It);
		if (ensure(AttributeComponent != nullptr) && AttributeComponent->IsAlive())
		{
			++AliveBotsNum;
		}
	}

	int32 AliveBotsMaxCount = 0;
	if (DifficultyCurve)
	{
		AliveBotsMaxCount = static_cast<int32>(DifficultyCurve->GetFloatValue(GetWorld()->TimeSeconds));
	}

	UE_LOG(LogTemp, Log, TEXT("Found %i alive bots"), AliveBotsNum);

	if (AliveBotsNum >= AliveBotsMaxCount)
	{
		UE_LOG(LogTemp, Log, TEXT("Maximum bot capacity has reached."));
		return;
	}

	auto* QueryInstance = UEnvQueryManager::RunEQSQuery(
		this,
		SpawnBotQuery,
		this,
		EEnvQueryRunMode::RandomBest5Pct,
		nullptr);

	if (ensure(QueryInstance != nullptr))
	{
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnBotSpawnQueryCompleted);
	}
}

void ASGameModeBase::OnBotSpawnQueryCompleted(
	UEnvQueryInstanceBlueprintWrapper* QueryInstance,
	EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn bot EQS query failed"));
		return;
	}

	auto Locations = QueryInstance->GetResultsAsLocations();
	if (!Locations.IsValidIndex(0))
	{
		return;
	}

	auto* SpawnedBot = GetWorld()->SpawnActor<AActor>(SpawnMinionBotClass, Locations[0], FRotator::ZeroRotator);
	if (ensure(SpawnedBot != nullptr))
	{
		DrawDebugSphere(GetWorld(), SpawnedBot->GetActorLocation(), 50, 20, FColor::Blue, false, 60);
	}
}
