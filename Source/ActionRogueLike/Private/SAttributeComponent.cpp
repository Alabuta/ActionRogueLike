// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"


bool USAttributeComponent::ApplyHealthChange(float Delta)
{
	Health += Delta;

	OnHealthChange.Broadcast(nullptr, this, Health, Delta);
	
	return true;
}

bool USAttributeComponent::IsAlive() const
{
	return Health > 0.f;
}

