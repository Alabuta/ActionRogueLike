// Fill out your copyright notice in the Description page of Project Settings.


#include "SItemChest.h"

ASItemChest::ASItemChest()
{
	PrimaryActorTick.bCanEverTick = true;

	BaseMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseStaticMesh"));
	RootComponent = BaseMeshComponent;

	LidMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LidStaticMesh"));
	LidMeshComponent->SetupAttachment(RootComponent);
}

void ASItemChest::Interact_Implementation(APawn* InstigatorPawn)
{
	LidMeshComponent->SetRelativeRotation(FRotator(TargetPitch, 0, 0));
}

void ASItemChest::BeginPlay()
{
	Super::BeginPlay();
}

void ASItemChest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

