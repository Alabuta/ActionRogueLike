// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SActionComponent.h"

#include "ActionRogueLike/ActionRogueLike.h"
#include "Actions/SAction.h"
#include "Algo/Find.h"
#include "Engine/ActorChannel.h"
#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"


USActionComponent::USActionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}

bool USActionComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	auto bDataSerialized = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (auto* Action : Actions)
	{
		if (IsValid(Action))
		{
			bDataSerialized |= Channel->ReplicateSubobject(Action, *Bunch, *RepFlags);
		}
	}

	return bDataSerialized;
}

void USActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/*auto DebugMessage = FString::Printf(
		TEXT("Action Component Tick: %s | %s"),
		*GetNameSafe(GetOwner()),
		*ActiveGameplayTags.ToStringSimple()); 
	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::White, MoveTemp(DebugMessage));*/

	for (const auto* Action : Actions)
	{
		const auto Color = Action->IsRunning() ? FColor::Blue : FColor::White;

		const auto Message = FString::Printf(
			TEXT("[%s] Action: %s | IsRunning: %s | Outer: %s"),
			*GetNameSafe(GetOwner()),
			*Action->ActionName.ToString(),
			Action->IsRunning() ? TEXT("true") : TEXT("false"),
			*GetNameSafe(Action->GetOuter()));

		LogOnScreen(this, Message, Color, 0.f);
	}
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

	if (auto* Action = NewObject<USAction>(GetOwner(), ActionClass); ensure(IsValid(Action)))
	{
		Action->Initialize(this);

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

	if (GetOwner()->HasAuthority())
	{
		for (auto&& ActionClass : GrantedActions)
		{
			AddAction(GetOwner(), ActionClass);
		}
	}
}

void USActionComponent::ServerStartAction_Implementation(AActor* Instigator, FName ActionName)
{
	StartActionByName(Instigator, ActionName);
}

void USActionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Actions);
}
