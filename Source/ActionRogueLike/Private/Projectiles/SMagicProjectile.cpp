// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectiles/SMagicProjectile.h"

#include "SGameplayFunctionLibrary.h"
#include "Components/SActionComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/ProjectileMovementComponent.h"


ASMagicProjectile::ASMagicProjectile()
{
	ColliderComponent->SetSphereRadius(20.f);
	ColliderComponent->OnComponentBeginOverlap.AddDynamic(this, &ASMagicProjectile::OnActorOverlap);

	InitialLifeSpan = 10.0f;
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

	auto* ActionComponent = OtherActor->GetComponentByClass<USActionComponent>();

	if (!bAlreadyReflected)
	{
		if (IsValid(ActionComponent) && ActionComponent->ActiveGameplayTags.HasTag(ParryTag))
		{
			bAlreadyReflected = true;
			MovementComponent->Velocity *= -1.f;
			SetInstigator(Cast<APawn>(OtherActor));
			return;
		}
	}

	const auto bApplyDirectionalDamage = USGameplayFunctionLibrary::ApplyDirectionalDamage(
		GetInstigator(),
		OtherActor,
		DamageValue,
		SweepResult);

	if (bApplyDirectionalDamage)
	{
		Explode();

		if (IsValid(ActionComponent) && DamageEffect && HasAuthority())
		{
			ActionComponent->AddAction(GetInstigator(), DamageEffect);
		}
	}
}
