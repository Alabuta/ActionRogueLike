// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FOnCreditsChangedDelegate,
	ASPlayerState*, PlayerState,
	const int32, NewCredits,
	const int32, Delta);

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable, Category="Credits")
	FOnCreditsChangedDelegate OnCreditsChanged;

	UFUNCTION(BlueprintCallable, Category="Credits")
	int32 GetCredits() const;

	UFUNCTION(BlueprintCallable, Category="Credits")
	void AddCredits(const int32 Value);

	UFUNCTION(BlueprintCallable, Category="Credits")
	bool TryRemoveCredits(const int32 Value);

protected:

	UPROPERTY(EditDefaultsOnly, Category="Credits")
	int32 Credits{0};
};
