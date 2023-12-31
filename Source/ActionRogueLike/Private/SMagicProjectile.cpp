// Fill out your copyright notice in the Description page of Project Settings.


#include "SMagicProjectile.h"

#include "SAttributeComponent.h"
#include "Components/SphereComponent.h"


ASMagicProjectile::ASMagicProjectile()
{
	ColliderComponent->SetSphereRadius(20.f);
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
	if (!OtherActor)
		return;

	if (OtherActor == GetInstigator())
		return;

	auto* ActorComponent = OtherActor->GetComponentByClass(USAttributeComponent::StaticClass());
	auto* AttributeComponent = Cast<USAttributeComponent>(ActorComponent);
	if (!AttributeComponent)
		return;

	AttributeComponent->ApplyHealthChange(-HealthDamageValue);

	Explode();
}
