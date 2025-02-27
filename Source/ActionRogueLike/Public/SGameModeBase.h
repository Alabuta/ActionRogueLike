// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "GameFramework/GameModeBase.h"
#include "PickUpItems/SPickUpItemBase.h"
#include "SGameModeBase.generated.h"

class USSaveGame;
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

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void StartPlay() override;

	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	virtual void OnActorKilled(AActor* VictimActor, AActor* KillerActor);

	UFUNCTION(BlueprintCallable, Category="SaveGame")
	void WriteSaveGame();

	void LoadSaveGame();

	UFUNCTION()
	void RespawnPlayerElapsed(AController* Controller);

	UFUNCTION(Exec)
	void KillAll();

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AI")
	TObjectPtr<UCurveFloat> DifficultyCurve{nullptr};

	UPROPERTY(EditDefaultsOnly, Category="AI")
	TObjectPtr<UEnvQuery> FindBotSpawnQuery{nullptr};

	UPROPERTY(EditAnywhere, Category="AI")
	TSubclassOf<AActor> MinionBotClass{nullptr};

	UPROPERTY(EditAnywhere, Category="PickUpItems")
	TArray<TSubclassOf<ASPickUpItemBase>> PickUpItemClasses;

	UPROPERTY(EditAnywhere, Category="PickUpItems")
	TObjectPtr<UEnvQuery> PickUpItemSpawnQuery;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	float SpawnTimeInterval{2.f};

	UPROPERTY(EditDefaultsOnly, Category="PickUpItems")
	int32 DesiredPickUpItemsCount{10};

	UPROPERTY(EditDefaultsOnly, Category="PickUpItems", meta=(ClampMin="0.1", ClampMax="1000", Units="cm"))
	float RequiredPickUpItemsDistance{500.f};

	UPROPERTY(EditDefaultsOnly, Category="Credits")
	int32 CreditsPerKill{200};

	FTimerHandle TimerHandle_SpawnBots;

private:

	UPROPERTY()
	USSaveGame* CurrentSaveGame{nullptr};

	FString SlotName{TEXTVIEW("SaveGame01")};

	UFUNCTION()
	void SpawnBotTimerElapsed();

	UFUNCTION()
	void OnFindBotSpawnQueryCompleted(
		UEnvQueryInstanceBlueprintWrapper* QueryInstance,
		EEnvQueryStatus::Type QueryStatus);

	UFUNCTION()
	void OnPickUpItemSpawnQueryCompleted(
		UEnvQueryInstanceBlueprintWrapper* QueryInstance,
		EEnvQueryStatus::Type QueryStatus);
};
