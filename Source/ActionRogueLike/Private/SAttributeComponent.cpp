// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"


bool USAttributeComponent::ApplyHealthChange(float Delta)
{
	Health += Delta;
	return true;
}

