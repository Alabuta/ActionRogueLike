// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "GameFramework/GameModeBase.h"
#include "SGameModeBase.generated.h"

class UEnvQuery;
/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	ASGameModeBase();

	virtual void StartPlay() override;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AI")
	TObjectPtr<UCurveFloat> DifficultyCurve;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	TObjectPtr<UEnvQuery> SpawnBotQuery;

	UPROPERTY(EditAnywhere, Category="AI")
	TSubclassOf<AActor> SpawnMinionBotClass;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	float SpawnTimeInterval = 2.f;
	
	FTimerHandle TimerHandle_SpawnBots;

private:

	UFUNCTION()
	void SpawnBot();

	UFUNCTION()
	void OnBotSpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);
};
