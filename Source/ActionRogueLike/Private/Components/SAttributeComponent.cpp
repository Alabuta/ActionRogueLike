﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SAttributeComponent.h"

#include "SGameModeBase.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"


namespace SConsoleVariables
{
	TAutoConsoleVariable CVarDamageMultiplier(
		TEXT("su.DamageMultiplier"),
		1.f,
		TEXT("Global damage multiplier for `AttributeComponent`"),
		ECVF_Cheat);
}

USAttributeComponent::USAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	Health = HealthMax;
}

bool USAttributeComponent::ApplyHealthChange(AActor* InstigatorActor, float Delta)
{
	if (const auto* OwnerActor = GetOwner(); !OwnerActor->CanBeDamaged() && Delta < 0.f)
	{
		return false;
	}

	if (Delta < 0.f)
	{
		Delta *= SConsoleVariables::CVarDamageMultiplier.GetValueOnGameThread();
	}

	Delta = FMath::Clamp(Health + Delta, 0, HealthMax) - Health;
	Health += Delta;

	OnHealthChange.Broadcast(InstigatorActor, this, Health, Delta);

	if (Delta < 0.f && Health == 0.f)
    {
	    if (auto* GameMode = GetWorld()->GetAuthGameMode<ASGameModeBase>(); IsValid(GameMode))
		{
			GameMode->OnActorKilled(GetOwner(), InstigatorActor);
		}
    }

	return Delta != 0;
}

bool USAttributeComponent::IsAlive() const
{
	return Health > 0.f;
}

bool USAttributeComponent::IsFullHealth() const
{
	return Health == HealthMax;
}

float USAttributeComponent::GetHealth() const
{
	return Health;
}

float USAttributeComponent::GetHealthMax() const
{
	return HealthMax;
}

float USAttributeComponent::GetHealthRatio() const
{
	return Health / HealthMax;
}

void USAttributeComponent::Kill(AActor* InstigatorActor)
{
	if (IsAlive())
	{
		ApplyHealthChange(InstigatorActor, -Health);
	}
}

USAttributeComponent* USAttributeComponent::GetAttributeComponent(const AActor* FromActor)
{
	return IsValid(FromActor) ? FromActor->GetComponentByClass<USAttributeComponent>() : nullptr;
}

bool USAttributeComponent::IsActorAlive(const AActor* Actor)
{
	if (const auto* AttributeComponent = GetAttributeComponent(Actor); IsValid(AttributeComponent))
	{
		return AttributeComponent->IsAlive();
	}

	return false;
}
