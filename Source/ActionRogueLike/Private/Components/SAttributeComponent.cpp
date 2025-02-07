// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SAttributeComponent.h"

#include "GameFramework/Actor.h"


USAttributeComponent::USAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	Health = HealthMax;
}

bool USAttributeComponent::ApplyHealthChange(AActor* InstigatorActor, float Delta)
{
	if (const auto* OwnerActor = GetOwner(); !OwnerActor->CanBeDamaged())
	{
		return false;
	}
	
	Delta = FMath::Clamp(Health + Delta, 0, HealthMax) - Health;
	Health += Delta;

	OnHealthChange.Broadcast(InstigatorActor, this, Health, Delta);

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
