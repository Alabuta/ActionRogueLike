// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUpItems/SPickUpItem_HealthPotion.h"

#include "Components/SAttributeComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "GameFramework/Pawn.h"
#include "Player/SPlayerState.h"


#define LOCTEXT_NAMESPACE "InteractableActors"

ASPickUpItem_HealthPotion::ASPickUpItem_HealthPotion()
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	StaticMeshComponent->SetupAttachment(RootComponent);
}

FText ASPickUpItem_HealthPotion::GetInteractText_Implementation(APawn* InstigatorPawn)
{
	const auto* AttributeComponent = USAttributeComponent::GetAttributeComponent(InstigatorPawn);
	if (!ensure(IsValid(AttributeComponent)))
	{
		return FText::GetEmpty();
	}

	if (AttributeComponent->IsFullHealth())
	{
		return LOCTEXT("HealthPotion_FullHealth", "Health is full");
	}

	return FText::Format(
		LOCTEXT("HealthPotion_InteractMessage", "Cost {0} Credits. Restore health to full."),
		CreditCost);
}

void ASPickUpItem_HealthPotion::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!ensure(IsValid(InstigatorPawn)))
	{
		return;
	}

	auto* AttributeComponent = USAttributeComponent::GetAttributeComponent(InstigatorPawn);
	if (!ensure(IsValid(AttributeComponent)))
	{
		return;
	}

	if (AttributeComponent->IsFullHealth())
	{
		return;
	}

	auto* PlayerState = InstigatorPawn->GetPlayerState<ASPlayerState>();
	if (!ensure(IsValid(PlayerState)))
	{
		return;
	}

	if (PlayerState->TryRemoveCredits(CreditCost) &&
		AttributeComponent->ApplyHealthChange(this, AttributeComponent->GetHealthMax()))
	{
		HideAndCooldown();
	}
}

#undef NSLOCTEXT_NAMESPACE
