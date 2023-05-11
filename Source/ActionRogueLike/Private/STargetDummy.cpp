// Fill out your copyright notice in the Description page of Project Settings.


#include "STargetDummy.h"

#include "SAttributeComponent.h"


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
	float NewHealth,
	float Delta)
{
	if (Delta >= 0.f)
		return;

	StaticMeshComponent->SetScalarParameterValueOnMaterials("TimeToHit", GetWorld()->TimeSeconds);
}

