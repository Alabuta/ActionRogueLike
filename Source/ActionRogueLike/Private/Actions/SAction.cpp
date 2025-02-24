// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/SAction.h"

#include "ActionRogueLike/ActionRogueLike.h"
#include "Components/SActionComponent.h"
#include "GameFramework/Actor.h"
#include "Logging/StructuredLog.h"
#include "Net/UnrealNetwork.h"


void USAction::StartAction_Implementation(AActor* Instigator)
{
	// UE_LOGFMT(LogTemp, Log, "Started action {0}", ActionName);
	LogOnScreen(this, FString::Printf(TEXT("Started: %s"), *ActionName.ToString()), FColor::Green);

	if (auto* ActionComponent = GetOwningComponent(); ensureAlways(IsValid(ActionComponent)))
	{
		ActionComponent->ActiveGameplayTags.AppendTags(GrantsTags);
		bIsRunning = true;
	}
}

void USAction::StopAction_Implementation(AActor* Instigator)
{
	// UE_LOGFMT(LogTemp, Log, "Stopped action {0}", ActionName);
	LogOnScreen(this, FString::Printf(TEXT("Stopped: %s"), *ActionName.ToString()), FColor::White);

	// ensureAlways(bIsRunning);

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

void USAction::OnRep_IsRunning()
{
	if (bIsRunning)
	{
		StartAction(nullptr);
	}
	else
	{
		StopAction(nullptr);
	}
}

void USAction::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, OwningActionComponent);
	DOREPLIFETIME(ThisClass, bIsRunning);
}
