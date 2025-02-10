// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameplayFunctionLibrary.h"

#include "Components/PrimitiveComponent.h"
#include "Components/SAttributeComponent.h"
#include "Engine/HitResult.h"


bool USGameplayFunctionLibrary::ApplyDamage(AActor* DamageCauser, AActor* TargetActor, const float DamageAmount)
{
	auto* AttributeComponent = USAttributeComponent::GetAttributeComponent(TargetActor);
	return IsValid(AttributeComponent) ? AttributeComponent->ApplyHealthChange(DamageCauser, -DamageAmount) : false;
}

bool USGameplayFunctionLibrary::ApplyDirectionalDamage(
	AActor* DamageCauser,
	AActor* TargetActor,
	const float DamageAmount,
	const FHitResult& HitResult)
{
	if (ApplyDamage(DamageCauser, TargetActor, DamageAmount))
	{
		if (auto* HitPrimitiveComponent = HitResult.GetComponent();
			IsValid(HitPrimitiveComponent) && HitPrimitiveComponent->IsSimulatingPhysics(HitResult.BoneName))
		{
			const auto ImpactNormal = (HitResult.ImpactPoint - DamageCauser->GetActorLocation()).GetSafeNormal();
			HitPrimitiveComponent->AddImpulseAtLocation(
				ImpactNormal * 300'000.f,
				HitResult.ImpactPoint,
				HitResult.BoneName);
		}

		return true;
	}

	return false;
}
