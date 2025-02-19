// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUpItems/SPickUpItem_Action.h"

#include "Actions/SAction.h"
#include "Components/SActionComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"


void ASPickUpItem_Action::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!ensure(IsValid(ActionClassToGrant)))
	{
		return;
	}

	auto* ActionComponent = InstigatorPawn->GetComponentByClass<USActionComponent>();
	if (!IsValid(ActionComponent))
	{
		return;
	}

	if (ActionComponent->GetAction(ActionClassToGrant) != nullptr)
	{
		auto DebugMessage = FString::Printf(TEXT("Action [%s] is already granted"), *GetNameSafe(ActionClassToGrant));
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, MoveTemp(DebugMessage));
		return;
	}

	ActionComponent->AddAction(InstigatorPawn, ActionClassToGrant);
	HideAndCooldown();
}
