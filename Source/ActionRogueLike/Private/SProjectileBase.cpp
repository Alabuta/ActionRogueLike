// Fill out your copyright notice in the Description page of Project Settings.


#include "SProjectileBase.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"


ASProjectileBase::ASProjectileBase()
{
	PrimaryActorTick.bCanEverTick = true;

	ColliderComponent = CreateDefaultSubobject<USphereComponent>(TEXT("ColliderComponent"));
	ColliderComponent->SetCollisionProfileName("Projectile");
	SetRootComponent(ColliderComponent);

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComponent"));
	MovementComponent->ProjectileGravityScale = 0.f;
	MovementComponent->InitialSpeed = 1'000.f;
	MovementComponent->bRotationFollowsVelocity = true;
	MovementComponent->bInitialVelocityInLocalSpace = true;

	VfxComponent = CreateDefaultSubobject<UParticleSystemComponent>("VfxComponent");
	VfxComponent->SetupAttachment(RootComponent);
}

void ASProjectileBase::BeginPlay()
{
	Super::BeginPlay();
}

void ASProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
