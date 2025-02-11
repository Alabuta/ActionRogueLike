// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/SAction.h"

#include "Components/SActionComponent.h"
#include "Logging/StructuredLog.h"


void USAction::StartAction_Implementation(AActor* Instigator)
{
	UE_LOGFMT(LogTemp, Log, "Running action {0}", ActionName);
}

void USAction::StopAction_Implementation(AActor* Instigator)
{
	UE_LOGFMT(LogTemp, Log, "Stopped action {0}", ActionName);
}

UWorld* USAction::GetWorld() const
{
	// Outer is set to the component that owns this action
	const auto* ActionComponent = Cast<USActionComponent>(GetOuter());
	return IsValid(ActionComponent) ? ActionComponent->GetWorld() : nullptr;
}
