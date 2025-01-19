// Fill out your copyright notice in the Description page of Project Settings.


#include "Props/SItemChest.h"

#include "Components/StaticMeshComponent.h"


ASItemChest::ASItemChest()
{
	PrimaryActorTick.bCanEverTick = true;

	BaseMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseStaticMesh"));
	SetRootComponent(BaseMeshComponent);

	LidMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LidStaticMesh"));
	LidMeshComponent->SetupAttachment(RootComponent);
}

void ASItemChest::Interact_Implementation(APawn* InstigatorPawn)
{
	LidMeshComponent->SetRelativeRotation(FRotator(TargetPitch, 0, 0));
}
