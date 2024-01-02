// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectiles/SProjectileBase.h"

#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"


ASProjectileBase::ASProjectileBase()
{
	// PrimaryActorTick.bCanEverTick = true;

	ColliderComponent = CreateDefaultSubobject<USphereComponent>(TEXT("ColliderComponent"));
	ColliderComponent->SetCollisionProfileName(TEXT("Projectile"));
	ColliderComponent->OnComponentHit.AddDynamic(this, &ASProjectileBase::OnActorHit);

	SetRootComponent(ColliderComponent);

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComponent"));
	MovementComponent->ProjectileGravityScale = 0.f;
	MovementComponent->InitialSpeed = 8'000.f;
	MovementComponent->bRotationFollowsVelocity = true;
	MovementComponent->bInitialVelocityInLocalSpace = true;

	VfxComponent = CreateDefaultSubobject<UParticleSystemComponent>("VfxComponent");
	VfxComponent->SetupAttachment(RootComponent);

	FlightAudioComponent = CreateDefaultSubobject<UAudioComponent>("FlightAudioComponent");
	FlightAudioComponent->SetupAttachment(RootComponent);
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

void ASProjectileBase::ApplyHitFX() const
{
	UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVfx, GetActorLocation(), GetActorRotation());

	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}

	UGameplayStatics::PlayWorldCameraShake(
		this,
		ImpactCameraShake,
		GetActorLocation(),
		ImpactCameraShakeInnerRadius,
		ImpactCameraShakeOuterRadius);
}

void ASProjectileBase::Explode_Implementation()
{
	if (!ensure(IsValid(this)))
		return;

	ApplyHitFX();

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
