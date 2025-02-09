// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SInteractionComponent.h"

#include "CollisionQueryParams.h"
#include "CollisionShape.h"
#include "DrawDebugHelpers.h"
#include "SGameplayInterface.h"
#include "Engine/HitResult.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"


 namespace SConsoleVariables
{
	TAutoConsoleVariable CVarDebugDraw(
		TEXT("su.Interaction.DebugDraw"),
		false,
		TEXT("Enable debug draw for `InteractionComponent`"),
		ECVF_Cheat);
}

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

	const auto End = EyeLocation + EyeRotation.Vector() * 1000;

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	const auto* World = GetWorld();

	TArray<FHitResult> Hits;

	constexpr float Radius = 30.f;

	const auto Shape = FCollisionShape::MakeSphere(Radius);

	const auto bBlockingHit = World->SweepMultiByObjectType(
		Hits,
		EyeLocation,
		End,
		FQuat::Identity,
		ObjectQueryParams,
		Shape);

	const auto LineColor = bBlockingHit ? FColor::Green : FColor::Red;

#if ENABLE_DRAW_DEBUG
	if (SConsoleVariables::CVarDebugDraw.GetValueOnGameThread())
	{
		DrawDebugLine(World, EyeLocation, End, LineColor, false, 2.f, 0, 2.f);
	}
#endif

	for (const FHitResult& Hit : Hits)
	{
#if ENABLE_DRAW_DEBUG
		if (SConsoleVariables::CVarDebugDraw.GetValueOnGameThread())
		{
			DrawDebugSphere(World, Hit.ImpactPoint, Radius, 32, LineColor, false, 2.f);
		}
#endif

		auto* HitActor = Hit.GetActor();
		if (!IsValid(HitActor))
		{
			continue;
		}

		if (!HitActor->Implements<USGameplayInterface>())
		{
			continue;
		}

		auto* OwnerPawn = Cast<APawn>(OwnerActor);
		ISGameplayInterface::Execute_Interact(HitActor, OwnerPawn);

		break;
	}
}
