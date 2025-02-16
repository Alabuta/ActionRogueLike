// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actions/SAction.h"
#include "Engine/TimerHandle.h"
#include "SActionEffect.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API USActionEffect : public USAction
{
	GENERATED_BODY()

public:

	USActionEffect();

	virtual void StartAction_Implementation(AActor* Instigator) override;
	virtual void StopAction_Implementation(AActor* Instigator) override;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effect", meta=(Units="s"))
	float Duration{5.f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effect", meta=(Units="s"))
	float Period{1.f};

	UFUNCTION(BlueprintNativeEvent, Category="Effect")
	void ExecutePeriodicEffect(AActor* Instigator);

private:

	FTimerHandle TimerHandle_Duration;
	FTimerHandle TimerHandle_Period;
};
