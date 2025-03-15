// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUpItems/SPickUpItemBase.h"

#include "TimerManager.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"


ASPickUpItemBase::ASPickUpItemBase()
{
	ColliderComponent = CreateDefaultSubobject<USphereComponent>(TEXT("ColliderComponent"));
	ColliderComponent->SetCollisionProfileName(TEXT("PickUpItem"));

	SetRootComponent(ColliderComponent);

	bReplicates = true;
}

FText ASPickUpItemBase::GetInteractText_Implementation(APawn* InstigatorPawn)
{
	return FText::GetEmpty();
}

void ASPickUpItemBase::OnRep_IsActive()
{
	SetActorEnableCollision(bIsActive);

	// Set visibility on root and all children
	RootComponent->SetVisibility(bIsActive, true);
}

void ASPickUpItemBase::Show()
{
	SetState(true);
}

void ASPickUpItemBase::HideAndCooldown()
{
	SetState(false);

	if (RespawnTime > 0.f)
	{
		GetWorldTimerManager()
		   .SetTimer(
			   TimerHandle_RespawnTimer,
			   this,
			   &ASPickUpItemBase::Show,
			   RespawnTime);
	}
}

void ASPickUpItemBase::SetState(const bool bNewIsActive)
{
	bIsActive = bNewIsActive;
	OnRep_IsActive();
}

void ASPickUpItemBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bIsActive);
}
