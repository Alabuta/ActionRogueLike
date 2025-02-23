// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SActionComponent.h"

#include "Actions/SAction.h"
#include "Algo/Find.h"
#include "Engine/Engine.h"


USActionComponent::USActionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}

void USActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	auto DebugMessage = FString::Printf(
		TEXT("Action Component Tick: %s | %s"),
		*GetNameSafe(GetOwner()),
		*ActiveGameplayTags.ToStringSimple()); 
	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::White, MoveTemp(DebugMessage));
}

USAction* USActionComponent::GetAction(TSubclassOf<USAction> ActionClass) const
{
	auto* const* ActionPtr = Actions.FindByPredicate([ActionClass](const USAction* Action)
	{
		return IsValid(Action) && Action->GetClass() == ActionClass;
	});

	return ActionPtr != nullptr ? *ActionPtr : nullptr;
}

void USActionComponent::AddAction(AActor* Instigator, const TSubclassOf<USAction> ActionClass)
{
	if (!ensure(ActionClass))
	{
		return;
	}

	if (auto* Action = NewObject<USAction>(this, ActionClass); ensure(IsValid(Action)))
	{
		Actions.Add(Action);

		if (Action->bAutoStart && ensure(Action->CanStartAction(Instigator)))
		{
			Action->StartAction(Instigator);
		}
	}
}

bool USActionComponent::StartActionByName(AActor* Instigator, FName ActionName)
{
	const auto* ActionPtr = Algo::FindByPredicate(Actions, [Instigator, ActionName](const USAction* Action)
	{
		return IsValid(Action) && Action->ActionName == ActionName && Action->CanStartAction(Instigator);
	});

	if (ActionPtr != nullptr)
	{
		if (!GetOwner()->HasAuthority())
		{
			ServerStartAction(Instigator, ActionName);
		}

		(*ActionPtr)->StartAction(Instigator);
		return true;
	}

	return false;
}

bool USActionComponent::StopActionByName(AActor* Instigator, FName ActionName)
{
	const auto* ActionPtr = Algo::FindByPredicate(Actions, [ActionName](const USAction* Action)
	{
		return IsValid(Action) && Action->ActionName == ActionName && Action->IsRunning();
	});

	if (ActionPtr != nullptr)
	{
		(*ActionPtr)->StopAction(Instigator);
		return true;
	}

	return false;
}

void USActionComponent::RemoveAction(USAction* Action)
{
	if (!ensure(IsValid(Action) && !Action->IsRunning()))
	{
		return;
	}

	Actions.Remove(Action);
}

void USActionComponent::BeginPlay()
{
	Super::BeginPlay();

	for (auto&& ActionClass : GrantedActions)
	{
		AddAction(GetOwner(), ActionClass);
	}
}

void USActionComponent::ServerStartAction_Implementation(AActor* Instigator, FName ActionName)
{
	StartActionByName(Instigator, ActionName);
}

