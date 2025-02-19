// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUpItems/SPickUpItemBase.h"

#include "TimerManager.h"
#include "Components/SphereComponent.h"


ASPickUpItemBase::ASPickUpItemBase()
{
	ColliderComponent = CreateDefaultSubobject<USphereComponent>(TEXT("ColliderComponent"));
	ColliderComponent->SetCollisionProfileName(TEXT("PickUpItem"));

	SetRootComponent(ColliderComponent);
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
	SetActorEnableCollision(bNewIsActive);

	// Set visibility on root and all children
	RootComponent->SetVisibility(bNewIsActive, true);
}
