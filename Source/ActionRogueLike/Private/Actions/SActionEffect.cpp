// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/SActionEffect.h"

#include "TimerManager.h"
#include "Components/SActionComponent.h"
#include "Engine/World.h"


USActionEffect::USActionEffect()
{
	bAutoStart = true;
}

float USActionEffect::GetTimeRemaining() const
{
	const float EndTime = TimeStarted + Duration;
	return EndTime - GetWorld()->GetTimeSeconds();
}

void USActionEffect::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	if (Duration > 0.f)
	{
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUFunction(this, TEXT("StopAction"), Instigator);

		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Duration, TimerDelegate, Duration, false);
	}

	if (Period > 0.f)
	{
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUFunction(this, TEXT("ExecutePeriodicEffect"), Instigator);

		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Period, TimerDelegate, Period, true);
	}
}

void USActionEffect::StopAction_Implementation(AActor* Instigator)
{
	auto&& TimerManager = GetWorld()->GetTimerManager();

	if (TimerManager.GetTimerRemaining(TimerHandle_Period) < KINDA_SMALL_NUMBER)
	{
		ExecutePeriodicEffect(Instigator);
	}

	Super::StopAction_Implementation(Instigator);

	TimerManager.ClearTimer(TimerHandle_Duration);
	TimerManager.ClearTimer(TimerHandle_Period);

	if (auto* ActionComponent = GetOwningComponent(); IsValid(ActionComponent))
	{
		ActionComponent->RemoveAction(this);
	}
}

void USActionEffect::ExecutePeriodicEffect_Implementation(AActor* Instigator)
{
}
