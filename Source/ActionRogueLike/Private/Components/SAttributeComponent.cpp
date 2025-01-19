// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SAttributeComponent.h"

#include "GameFramework/Actor.h"


bool USAttributeComponent::ApplyHealthChange(AActor* InstigatorActor, float Delta)
{
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

float USAttributeComponent::GetHealthMax() const
{
	return HealthMax;
}

USAttributeComponent* USAttributeComponent::GetAttributeComponent(const AActor* FromActor)
{
	return FromActor != nullptr ? FromActor->GetComponentByClass<USAttributeComponent>() : nullptr;
}

bool USAttributeComponent::IsActorAlive(const AActor* Actor)
{
	if (const auto* AttributeComponent = GetAttributeComponent(Actor); AttributeComponent != nullptr)
	{
		return AttributeComponent->IsAlive();
	}

	return false;
}
