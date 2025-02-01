// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"

#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "TimerManager.h"
#include "AI/SAICharacter.h"
#include "Components/SAttributeComponent.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "Logging/StructuredLog.h"


void ASGameModeBase::StartPlay()
{
	Super::StartPlay();

	if (MinionBotClass)
	{
		GetWorldTimerManager()
			.SetTimer(
				TimerHandle_SpawnBots,
				this,
				&ASGameModeBase::SpawnBot,
				SpawnTimeInterval,
				true);
	}
}

void ASGameModeBase::SpawnBot()
{
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
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnFindBotSpawnQueryComplete);
	}
}

void ASGameModeBase::OnFindBotSpawnQueryComplete(
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
