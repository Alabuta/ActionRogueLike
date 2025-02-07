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

	virtual void StartPlay() override;

	UFUNCTION(Exec)
	void KillAll();

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AI")
	TObjectPtr<UCurveFloat> DifficultyCurve{nullptr};

	UPROPERTY(EditDefaultsOnly, Category="AI")
	TObjectPtr<UEnvQuery> FindBotSpawnQuery{nullptr};

	UPROPERTY(EditAnywhere, Category="AI")
	TSubclassOf<AActor> MinionBotClass{nullptr};

	UPROPERTY(EditDefaultsOnly, Category="AI")
	float SpawnTimeInterval{2.f};

	FTimerHandle TimerHandle_SpawnBots;

private:

	UFUNCTION()
	void SpawnBotTimerElapsed();

	UFUNCTION()
	void OnFindBotSpawnQueryComplete(
		UEnvQueryInstanceBlueprintWrapper* QueryInstance,
		EEnvQueryStatus::Type QueryStatus);
};
