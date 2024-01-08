// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"

#include "EngineUtils.h"
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

	int32 AliveBotsNum = 0;
	for (TActorIterator<ASAICharacter> It{GetWorld()}; It; ++It)
	{
		const auto* AttributeComponent = It->GetComponentByClass<USAttributeComponent>();
		AliveBotsNum += AttributeComponent != nullptr && AttributeComponent->IsAlive() ? 1 : 0;
	}

	int32 AliveBotsMaxCount = 0;
	if (DifficultyCurve)
	{
		AliveBotsMaxCount = static_cast<int32>(DifficultyCurve->GetFloatValue(GetWorld()->TimeSeconds));
	}

	if (AliveBotsNum >= AliveBotsMaxCount)
	{
		return;
	}

	auto Locations = QueryInstance->GetResultsAsLocations();
	if (!Locations.IsValidIndex(0))
	{
		return;
	}

	GetWorld()->SpawnActor<AActor>(SpawnMinionBotClass, Locations[0], FRotator::ZeroRotator);
}
