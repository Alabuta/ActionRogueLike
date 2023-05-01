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

	AActor* OwnerActor = GetOwner();
	OwnerActor->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	FVector End = EyeLocation + EyeRotation.Vector() * 1000;

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	UWorld* World = GetWorld();

	TArray<FHitResult> Hits;
	
	constexpr float Radius = 30.f;
	
	FCollisionShape Shape;
	Shape.SetSphere(Radius);
	
	bool bBlockingHit = World->SweepMultiByObjectType(Hits, EyeLocation, End, FQuat::Identity, ObjectQueryParams, Shape);
	FColor LineColor = bBlockingHit ? FColor::Green : FColor::Red;
	
	DrawDebugLine(World, EyeLocation, End, LineColor, false, 2.f, 0, 2.f);

	for (const FHitResult& Hit : Hits)
	{
		DrawDebugSphere(World, Hit.ImpactPoint, Radius, 32, LineColor, false, 2.f);
		
		AActor* HitActor = Hit.GetActor();
		if (HitActor == nullptr)
			continue;

		if (!HitActor->Implements<USGameplayInterface>())
			continue;

		APawn* OwnerPawn = Cast<APawn>(OwnerActor);
		ISGameplayInterface::Execute_Interact(HitActor, OwnerPawn);

		break;
	}
}


void USInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
}


void USInteractionComponent::TickComponent(float DeltaTime,
                                           ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
