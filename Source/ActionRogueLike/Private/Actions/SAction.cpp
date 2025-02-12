// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/SAction.h"

#include "Components/SActionComponent.h"
#include "Logging/StructuredLog.h"


void USAction::StartAction_Implementation(AActor* Instigator)
{
	UE_LOGFMT(LogTemp, Log, "Running action {0}", ActionName);

	if (auto* ActionComponent = GetOwningComponent(); ensureAlways(IsValid(ActionComponent)))
	{
		ActionComponent->ActiveGameplayTags.AppendTags(GrantsTags);
		bIsRunning = true;
	}
}

void USAction::StopAction_Implementation(AActor* Instigator)
{
	UE_LOGFMT(LogTemp, Log, "Stopped action {0}", ActionName);

	ensureAlways(bIsRunning);

	if (auto* ActionComponent = GetOwningComponent(); ensureAlways(IsValid(ActionComponent)))
	{
		ActionComponent->ActiveGameplayTags.RemoveTags(GrantsTags);
	}

	bIsRunning = false;
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
	return bIsRunning;	
}

UWorld* USAction::GetWorld() const
{
	// Outer is set to the component that owns this action
	const auto* ActionComponent = GetOwningComponent();
	return IsValid(ActionComponent) ? ActionComponent->GetWorld() : nullptr;
}

USActionComponent* USAction::GetOwningComponent() const
{
	return Cast<USActionComponent>(GetOuter());
}
