// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SAttributeComponent.h"


bool USAttributeComponent::ApplyHealthChange(float Delta)
{
	Delta = FMath::Clamp(Health + Delta, 0, HealthMax) - Health;
	Health += Delta;

	OnHealthChange.Broadcast(nullptr, this, Health, Delta);

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
