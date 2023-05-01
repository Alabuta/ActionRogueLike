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

	FHitResult Hit;
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	UWorld* World = GetWorld();
	DrawDebugLine(World, EyeLocation, End, FColor::Red, false, 2.f, 0, 2.f);
	World->LineTraceSingleByObjectType(Hit, EyeLocation, End, ObjectQueryParams);

	AActor* HitActor = Hit.GetActor();
	if (HitActor == nullptr)
		return;

	if (!HitActor->Implements<USGameplayInterface>())
		return;

	APawn* OwnerPawn = Cast<APawn>(OwnerActor);
	ISGameplayInterface::Execute_Interact(HitActor, OwnerPawn);

	
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
