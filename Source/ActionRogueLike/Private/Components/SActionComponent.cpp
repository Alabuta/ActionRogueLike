// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SActionComponent.h"

#include <ranges>
#include "Actions/SAction.h"
#include "Algo/Find.h"


USActionComponent::USActionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USActionComponent::BeginPlay()
{
	Super::BeginPlay();

	for (auto&& ActionClass : GrantedActions)
	{
		AddAction(ActionClass);
	}
}

void USActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void USActionComponent::AddAction(const TSubclassOf<USAction> ActionClass)
{
	if (!ensure(ActionClass))
	{
		return;
	}

	if (auto* Action = NewObject<USAction>(this, ActionClass); ensure(IsValid(Action)))
	{
		Actions.Add(Action);
	}
}

bool USActionComponent::StartActionByName(AActor* Instigator, FName ActionName)
{
	const auto* ActionPtr = Algo::FindByPredicate(Actions, [ActionName](const USAction* Action)
	{
		return IsValid(Action) && Action->ActionName == ActionName;
	});

	if (ActionPtr != nullptr)
	{
		(*ActionPtr)->StartAction(Instigator);
		return true;
	}

	return false;
}

bool USActionComponent::StopActionByName(AActor* Instigator, FName ActionName)
{
	const auto* ActionPtr = Algo::FindByPredicate(Actions, [ActionName](const USAction* Action)
	{
		return IsValid(Action) && Action->ActionName == ActionName;
	});

	if (ActionPtr != nullptr)
	{
		(*ActionPtr)->StopAction(Instigator);
		return true;
	}

	return false;
}

