// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectiles/SMagicProjectile.h"

#include "SGameplayFunctionLibrary.h"
#include "Components/SAttributeComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Pawn.h"


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
	if (!IsValid(OtherActor) || OtherActor == GetInstigator())
	{
		return;
	}

	if (auto* AttributeComponent = OtherActor->GetComponentByClass<USAttributeComponent>(); IsValid(AttributeComponent))
	{
		AttributeComponent->ApplyHealthChange(GetInstigator(), -HealthDamageValue);
	}

	const auto bApplyDirectionalDamage = USGameplayFunctionLibrary::ApplyDirectionalDamage(
		GetInstigator(),
		OtherActor,
		HealthDamageValue,
		SweepResult);

	if (bApplyDirectionalDamage)
	{
		Explode();
	}
}
