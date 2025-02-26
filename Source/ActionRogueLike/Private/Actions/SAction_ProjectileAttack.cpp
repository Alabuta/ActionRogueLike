// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/SAction_ProjectileAttack.h"

#include "SCharacter.h"
#include "TimerManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"


void USAction_ProjectileAttack::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	auto* InstigatorCharacter = Cast<ACharacter>(Instigator);
	if (!IsValid(InstigatorCharacter))
	{
		return;
	}

	InstigatorCharacter->PlayAnimMontage(AttackAnim);

	UGameplayStatics::SpawnEmitterAttached(
		CastingVFX,
		InstigatorCharacter->GetMesh(),
		MeshSocketName,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		EAttachLocation::SnapToTarget);

	if (InstigatorCharacter->HasAuthority())
	{
		FTimerHandle TimerHandle_AttackDelay;
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUFunction(this, TEXT("AttackDelay_Elapsed"), InstigatorCharacter);

		GetWorld()->GetTimerManager()
		          .SetTimer(
			          TimerHandle_AttackDelay,
			          TimerDelegate,
			          AttackAnimDelay,
			          false); // :TODO: use animation events
	}
}

void USAction_ProjectileAttack::AttackDelay_Elapsed(ACharacter* InstigatorCharacter)
{
	if (!ensureAlways(ProjectileClass))
	{
		StopAction(InstigatorCharacter);
		return;
	}

	if (!IsValid(InstigatorCharacter))
	{
		StopAction(InstigatorCharacter);
		return;
	}

	auto* World = GetWorld();

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

	FCollisionShape Shape;
	Shape.SetSphere(20.f);

	FCollisionQueryParams AdditionalQueryParams;
	AdditionalQueryParams.AddIgnoredActor(InstigatorCharacter);

	auto TraceStart = InstigatorCharacter->GetPawnViewLocation();
	auto TraceEnd = TraceStart + InstigatorCharacter->GetControlRotation().Vector() * 5'000;

	const auto RightHandSocketLocation = InstigatorCharacter->GetMesh()->GetSocketLocation(MeshSocketName);

	FHitResult Hit;
	auto bBlockingHit = World->SweepSingleByObjectType(
		Hit,
		TraceStart,
		TraceEnd,
		FQuat::Identity,
		ObjectQueryParams,
		Shape,
		AdditionalQueryParams);

	auto SpawnRotation = bBlockingHit
							 ? FRotationMatrix::MakeFromX(Hit.ImpactPoint - RightHandSocketLocation).Rotator()
							 : FRotationMatrix::MakeFromX(TraceEnd - RightHandSocketLocation).Rotator();

	const auto SpawnTransform = FTransform(SpawnRotation, RightHandSocketLocation);

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParameters.Instigator = InstigatorCharacter;

	World->SpawnActor<AActor>(ProjectileClass, SpawnTransform, SpawnParameters);

	StopAction(InstigatorCharacter);
}
