// Fill out your copyright notice in the Description page of Project Settings.


#include "SDashProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"


ASDashProjectile::ASDashProjectile()
{
	MovementComponent->InitialSpeed = 6'000.f;
}

void ASDashProjectile::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(TimerHandle_DelayedDetonate, this, &ASDashProjectile::Explode, DetonateDelay);
}

void ASDashProjectile::Explode_Implementation()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_DelayedDetonate);

	UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVfx, GetActorLocation(), GetActorRotation());

	VfxComponent->DeactivateSystem();
	MovementComponent->StopMovementImmediately();

	SetActorEnableCollision(false);

	FTimerHandle TimerHandle_DelayedTeleport;
	GetWorldTimerManager().SetTimer(TimerHandle_DelayedTeleport, this, &ASDashProjectile::TeleportInstigator, TeleportDelay);
}

void ASDashProjectile::TeleportInstigator()
{
	auto* ActorToTeleport = GetInstigator();
	if (!ensure(ActorToTeleport))
		return;

	ActorToTeleport->TeleportTo(GetActorLocation(), ActorToTeleport->GetActorRotation());
}
