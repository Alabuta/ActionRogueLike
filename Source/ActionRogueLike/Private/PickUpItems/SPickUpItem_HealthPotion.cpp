// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUpItems/SPickUpItem_HealthPotion.h"

#include "Components/SAttributeComponent.h"


ASPickUpItem_HealthPotion::ASPickUpItem_HealthPotion()
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	StaticMeshComponent->SetupAttachment(RootComponent);
}

void ASPickUpItem_HealthPotion::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!ensure(InstigatorPawn != nullptr))
		return;

	auto* AttributeComponent = InstigatorPawn->GetComponentByClass<USAttributeComponent>();
	if (ensure(AttributeComponent != nullptr) && !AttributeComponent->IsFullHealth())
	{
		if (AttributeComponent->ApplyHealthChange(this, AttributeComponent->GetHealthMax()))
		{
			HideAndCooldown();
		}
	}
}
