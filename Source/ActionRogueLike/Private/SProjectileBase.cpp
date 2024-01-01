// Fill out your copyright notice in the Description page of Project Settings.


#include "SProjectileBase.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"


ASProjectileBase::ASProjectileBase()
{
	// PrimaryActorTick.bCanEverTick = true;

	ColliderComponent = CreateDefaultSubobject<USphereComponent>(TEXT("ColliderComponent"));
	ColliderComponent->SetCollisionProfileName("Projectile");
	ColliderComponent->OnComponentHit.AddDynamic(this, &ASProjectileBase::OnActorHit);

	SetRootComponent(ColliderComponent);

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComponent"));
	MovementComponent->ProjectileGravityScale = 0.f;
	MovementComponent->InitialSpeed = 8'000.f;
	MovementComponent->bRotationFollowsVelocity = true;
	MovementComponent->bInitialVelocityInLocalSpace = true;

	VfxComponent = CreateDefaultSubobject<UParticleSystemComponent>("VfxComponent");
	VfxComponent->SetupAttachment(RootComponent);
}

void ASProjectileBase::OnActorHit(
	UPrimitiveComponent* HitComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	FVector NormalImpulse,
	const FHitResult& Hit)
{
	Explode();
}

void ASProjectileBase::Explode_Implementation()
{
	if (!ensure(IsValid(this)))
		return;

	UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVfx, GetActorLocation(), GetActorRotation());

	VfxComponent->DeactivateSystem();

	MovementComponent->StopMovementImmediately();
	SetActorEnableCollision(false);
	
	Destroy();
}

void ASProjectileBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	// ColliderComponent->IgnoreActorWhenMoving(GetInstigator(), true);
}
