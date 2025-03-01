// Fill out your copyright notice in the Description page of Project Settings.


#include "Props/SItemChest.h"

#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"


ASItemChest::ASItemChest()
{
	PrimaryActorTick.bCanEverTick = true;

	BaseMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseStaticMesh"));
	SetRootComponent(BaseMeshComponent);

	LidMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LidStaticMesh"));
	LidMeshComponent->SetupAttachment(RootComponent);

	bReplicates = true;
}

void ASItemChest::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bLidOpened);
}

void ASItemChest::OnActorLoaded_Implementation()
{
	OnRep_LidOpened();
}

void ASItemChest::Interact_Implementation(APawn* InstigatorPawn)
{
	bLidOpened = !bLidOpened;
	OnRep_LidOpened();
}

void ASItemChest::OnRep_LidOpened()
{
 	const auto CurrentPitch = bLidOpened ? TargetPitch : 0.f;
	LidMeshComponent->SetRelativeRotation(FRotator{CurrentPitch, 0, 0});
}
