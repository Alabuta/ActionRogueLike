	// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SActionComponent.h"

#include "ActionRogueLike/ActionRogueLike.h"
#include "Actions/SAction.h"
#include "Algo/Find.h"
#include "Engine/ActorChannel.h"
#include "Engine/Engine.h"
#include "Logging/StructuredLog.h"
#include "Net/UnrealNetwork.h"


DECLARE_CYCLE_STAT(TEXT("StartActionByName"), STAT_StartActionByName, STATGROUP_STANFORD);


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

	auto* OwnerActor = GetOwner();
	if (!IsValid(OwnerActor))
	{
		return;
	}

	if (!OwnerActor->HasAuthority())
	{
		UE_LOGFMT(LogTemp, Warning, "client attempting to add action : [{0}]", GetNameSafe(ActionClass));
	    return;
	}

	if (auto* Action = NewObject<USAction>(OwnerActor, ActionClass); ensure(IsValid(Action)))
	{
		Action->Initialize(this);

		Actions.Add(Action);

		if (Action->bAutoStart && ensure(Action->CanStartAction(Instigator)))
		{
			TRACE_BOOKMARK(TEXT("StartAction::%s"), *GetNameSafe(Action));
			Action->StartAction(Instigator);
		}
	}
}

bool USActionComponent::StartActionByName(AActor* Instigator, FName ActionName)
{
	SCOPE_CYCLE_COUNTER(STAT_StartActionByName);

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
		if (!GetOwner()->HasAuthority())
		{
			ServerStopAction(Instigator, ActionName);
		}

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

	auto* Owner = GetOwner();

	if (!Owner->HasAuthority())
	{
		return;
	}

	for (auto&& ActionClass : GrantedActions)
	{
		AddAction(Owner, ActionClass);
	}
}

void USActionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	for (TArray<USAction*> ActionsCopy = Actions; auto* Action : ActionsCopy)
	{
		if (Action->IsRunning())
		{
			Action->StopAction(GetOwner());
		}
	}

	Super::EndPlay(EndPlayReason);
}

void USActionComponent::ServerStartAction_Implementation(AActor* Instigator, FName ActionName)
{
	StartActionByName(Instigator, ActionName);
}

void USActionComponent::ServerStopAction_Implementation(AActor* Instigator, FName ActionName)
{
    StopActionByName(Instigator, ActionName);
}

void USActionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Actions);
}
