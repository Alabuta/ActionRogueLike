// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUpItems/SPickUpItem_Credits.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "GameFramework/Pawn.h"
#include "Player/SPlayerState.h"


ASPickUpItem_Credits::ASPickUpItem_Credits()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	MeshComponent->SetupAttachment(RootComponent);
}

void ASPickUpItem_Credits::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!ensure(IsValid(InstigatorPawn)))
	{
		return;
	}

	auto* PlayerState = InstigatorPawn->GetPlayerState<ASPlayerState>();
	if (!ensure(IsValid(PlayerState)))
	{
		return;
	}

	PlayerState->AddCredits(CreditsAmount);

	HideAndCooldown();
}
