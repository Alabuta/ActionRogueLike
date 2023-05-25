// Fill out your copyright notice in the Description page of Project Settings.


#include "SMagicProjectile.h"

#include "SAttributeComponent.h"


ASMagicProjectile::ASMagicProjectile()
{
}

void ASMagicProjectile::OnActorOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!OtherActor)
		return;

	if (OtherActor == GetInstigator())
		return;

	USAttributeComponent* AttributeComponent = Cast<USAttributeComponent>(
		OtherActor->GetComponentByClass(USAttributeComponent::StaticClass()));

	if (!AttributeComponent)
		return;

	AttributeComponent->ApplyHealthChange(HealthDamageValue);
	Destroy();
}
