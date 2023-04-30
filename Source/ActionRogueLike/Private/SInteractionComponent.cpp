// Fill out your copyright notice in the Description page of Project Settings.


#include "SInteractionComponent.h"

#include "SGameplayInterface.h"

// Sets default values for this component's properties
USInteractionComponent::USInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void USInteractionComponent::PrimaryInteract()
{
	FVector EyeLocation;
	FRotator EyeRotation;

	AActor* CharacterActor = GetOwner();
	CharacterActor->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	FVector End = EyeLocation + EyeRotation.Vector() * 1000;

	FHitResult Hit;
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	
	GetWorld()->LineTraceSingleByObjectType(Hit, EyeLocation, End, ObjectQueryParams);

	AActor* HitActor = Hit.GetActor();
	if (HitActor == nullptr)
		return;

	if (!HitActor->Implements<ISGameplayInterface>())
		return;
	
	// APawn* CharacterPawn = Cast<APawn>(CharacterActor);
	// ISGameplayInterface::Execute_Interact(HitActor, CharacterPawn);
}


// Called when the game starts
void USInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void USInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
