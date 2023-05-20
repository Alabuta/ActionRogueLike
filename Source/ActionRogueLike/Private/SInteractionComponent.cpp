// Fill out your copyright notice in the Description page of Project Settings.


#include "SInteractionComponent.h"
#include "SGameplayInterface.h"

USInteractionComponent::USInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USInteractionComponent::PrimaryInteract()
{
	FVector EyeLocation;
	FRotator EyeRotation;

	auto* OwnerActor = GetOwner();
	OwnerActor->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	auto End = EyeLocation + EyeRotation.Vector() * 1000;

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	auto* World = GetWorld();

	TArray<FHitResult> Hits;
	
	constexpr float Radius = 30.f;
	
	FCollisionShape Shape;
	Shape.SetSphere(Radius);

	auto bBlockingHit = World->SweepMultiByObjectType(Hits, EyeLocation, End, FQuat::Identity, ObjectQueryParams, Shape);
	auto LineColor = bBlockingHit ? FColor::Green : FColor::Red;
	
	DrawDebugLine(World, EyeLocation, End, LineColor, false, 2.f, 0, 2.f);

	for (const FHitResult& Hit : Hits)
	{
		DrawDebugSphere(World, Hit.ImpactPoint, Radius, 32, LineColor, false, 2.f);
		
		auto* HitActor = Hit.GetActor();
		if (HitActor == nullptr)
			continue;

		if (!HitActor->Implements<USGameplayInterface>())
			continue;

		auto* OwnerPawn = Cast<APawn>(OwnerActor);
		ISGameplayInterface::Execute_Interact(HitActor, OwnerPawn);

		break;
	}
}
