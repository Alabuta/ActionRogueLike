// Fill out your copyright notice in the Description page of Project Settings.


#include "SMagicProjectile.h"

#include "SAttributeComponent.h"
#include "Components/SphereComponent.h"


ASMagicProjectile::ASMagicProjectile()
{
	ColliderComponent->OnComponentBeginOverlap.AddDynamic(this, &ASMagicProjectile::OnActorOverlap);
}

void ASMagicProjectile::OnActorOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (OtherActor == nullptr)
		return;

	USAttributeComponent* AttributeComponent = Cast<USAttributeComponent>(
		OtherActor->GetComponentByClass(USAttributeComponent::StaticClass()));

	if (AttributeComponent == nullptr)
		return;

	AttributeComponent->ApplyHealthChange(HealthDamageValue);
	Destroy();
}
