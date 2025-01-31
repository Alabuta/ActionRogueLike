// Fill out your copyright notice in the Description page of Project Settings.


#include "Props/SExplosiveBarrel.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/World.h"
#include "Logging/StructuredLog.h"
#include "PhysicsEngine/RadialForceComponent.h"


// Sets default values
ASExplosiveBarrel::ASExplosiveBarrel()
{
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->SetSimulatePhysics(true);
	StaticMeshComponent->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
	SetRootComponent(StaticMeshComponent);

	RadialForceComponent = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForce"));
	RadialForceComponent->SetupAttachment(StaticMeshComponent);
	RadialForceComponent->SetAutoActivate(false);
	RadialForceComponent->Radius = 750.f;
	RadialForceComponent->ImpulseStrength = 2'500.f;
	RadialForceComponent->bImpulseVelChange = false;
	RadialForceComponent->AddCollisionChannelToAffect(ECC_WorldDynamic);
}

void ASExplosiveBarrel::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	StaticMeshComponent->OnComponentHit.AddDynamic(this, &ASExplosiveBarrel::OnActorHit);
}

void ASExplosiveBarrel::OnActorHit(
	[[maybe_unused]] UPrimitiveComponent* HitComponent,
	AActor* OtherActor,
	[[maybe_unused]] UPrimitiveComponent* OtherHitComponent,
	[[maybe_unused]] FVector NormalImpulse,
	const FHitResult& Hit)
{
	RadialForceComponent->FireImpulse();

	UE_LOGFMT(LogTemp, Log, "OnActorHit in Explosive Barrel");
	UE_LOGFMT(
		LogTemp,
		Warning,
		"OtherActor: {0}, at game time: {1}",
		*GetNameSafe(OtherActor),
		GetWorld()->TimeSeconds);

	const auto CombinedString = FString::Printf(TEXT("Hit at location: %s"), *Hit.ImpactPoint.ToString());
	DrawDebugString(GetWorld(), Hit.ImpactPoint, CombinedString, nullptr, FColor::Green, 2.0f, true);
}
