// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/SAction.h"

#include "Logging/StructuredLog.h"


void USAction::StartAction_Implementation(AActor* Instigator)
{
	UE_LOGFMT(LogTemp, Log, "Running action {0}", ActionName);
}

void USAction::StopAction_Implementation(AActor* Instigator)
{
	UE_LOGFMT(LogTemp, Log, "Stopped action {0}", ActionName);
}
