// Fill out your copyright notice in the Description page of Project Settings.


#include "Props/STargetDummy.h"

#include "Components/SAttributeComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"


ASTargetDummy::ASTargetDummy()
{
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	SetRootComponent(StaticMeshComponent);

	AttributeComponent = CreateDefaultSubobject<USAttributeComponent>(TEXT("HealthAttribute"));
	AttributeComponent->OnHealthChange.AddDynamic(this, &ASTargetDummy::OnHealthChanged);
}

void ASTargetDummy::OnHealthChanged(
	AActor* InstigatorActor,
	USAttributeComponent* OwningComponent,
	const float NewHealth,
	const float Delta)
{
	if (Delta >= 0.f)
	{
		return;
	}

	StaticMeshComponent->SetScalarParameterValueOnMaterials(TEXT("TimeToHit"), GetWorld()->TimeSeconds);
}

