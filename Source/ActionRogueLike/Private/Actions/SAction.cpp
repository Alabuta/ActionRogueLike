// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/SAction.h"

#include "ActionRogueLike/ActionRogueLike.h"
#include "Animation/AnimInstance.h"
#include "Components/SActionComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Logging/StructuredLog.h"
#include "Net/UnrealNetwork.h"


void USAction::StartAction_Implementation(AActor* Instigator)
{
	UE_LOGFMT(LogTemp, Log, "Started action {0}", ActionName);

	if (auto* ActionComponent = GetOwningComponent(); ensureAlways(IsValid(ActionComponent)))
	{
		ActionComponent->ActiveGameplayTags.AppendTags(GrantsTags);
		ActionComponent->OnActionStarted.Broadcast(ActionComponent, this);

		RepData.Instigator = Instigator;
		RepData.bIsRunning = true;

		if (ActionComponent->GetOwnerRole() == ROLE_Authority)
        {
            TimeStarted = GetWorld()->GetTimeSeconds();
        }
	}
}

void USAction::StopAction_Implementation(AActor* Instigator)
{
	UE_LOGFMT(LogTemp, Log, "Stopped action {0}", ActionName);

	// ensureAlways(bIsRunning);

	if (auto* ActionComponent = GetOwningComponent(); ensureAlways(IsValid(ActionComponent)))
	{
		ActionComponent->ActiveGameplayTags.RemoveTags(GrantsTags);
		ActionComponent->OnActionStopped.Broadcast(ActionComponent, this);

		RepData.Instigator = Instigator;
		RepData.bIsRunning = false;
	}
}

bool USAction::CanStartAction_Implementation(AActor* Instigator) const
{
	if (IsRunning())
	{
		return false;
	}
	
	if (const auto* ActionComponent = GetOwningComponent(); !ensureAlways(IsValid(ActionComponent)))
	{
		return false;
	}
	else
	{
		return !ActionComponent->ActiveGameplayTags.HasAny(BlockedTags);
	}
}

bool USAction::IsRunning_Implementation() const
{
	return RepData.bIsRunning;	
}

void USAction::Initialize(USActionComponent* NewActionComponent)
{
	OwningActionComponent = NewActionComponent;
}

UWorld* USAction::GetWorld() const
{
	// Outer is set when creating the action via NewObject<T>
	const auto* Owner = Cast<AActor>(GetOuter());
	return IsValid(Owner) ? Owner->GetWorld() : nullptr;
}

USActionComponent* USAction::GetOwningComponent() const
{
	return OwningActionComponent;
}

void USAction::OnRep_RepData()
{
	if (RepData.bIsRunning)
	{
		StartAction(RepData.Instigator);
	}
	else
	{
		StopAction(RepData.Instigator);
	}
}

void USAction::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, OwningActionComponent);
	DOREPLIFETIME(ThisClass, RepData);
	DOREPLIFETIME(ThisClass, TimeStarted);
}
