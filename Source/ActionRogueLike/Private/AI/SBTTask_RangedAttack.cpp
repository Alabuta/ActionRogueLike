// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTTask_RangedAttack.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SAttributeComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"


EBTNodeResult::Type USBTTask_RangedAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const auto* AIController = OwnerComp.GetAIOwner();
	if (!IsValid(AIController))
	{
		return EBTNodeResult::Failed;
	}

	auto* Pawn = Cast<ACharacter>(AIController->GetPawn());
	if (!IsValid(Pawn))
	{
		return EBTNodeResult::Failed;
	}

	const auto* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!ensure(IsValid(BlackboardComponent)))
	{
		return EBTNodeResult::Failed;
	}

	const auto* TargetActor = Cast<AActor>(BlackboardComponent->GetValueAsObject(TEXT("TargetActor")));
	if (!IsValid(TargetActor))
	{
		return EBTNodeResult::Failed;
	}

	if (!USAttributeComponent::IsActorAlive(TargetActor))
	{
		return EBTNodeResult::Failed;
	}

	const auto PawnMuzzleLocation = Pawn->GetMesh()->GetSocketLocation(TEXT("Muzzle_01"));
	const auto Direction = TargetActor->GetActorLocation() - PawnMuzzleLocation;

	auto PawnMuzzleRotation = Direction.Rotation();
	PawnMuzzleRotation.Pitch += FMath::RandRange(0.f, MaxProjectileSpread);
	PawnMuzzleRotation.Yaw += FMath::RandRange(-MaxProjectileSpread, MaxProjectileSpread);

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParameters.Instigator = Pawn;

	const auto* SpawnedProjectile = GetWorld()->SpawnActor<AActor>(
		ProjectileClass,
		PawnMuzzleLocation,
		PawnMuzzleRotation,
		SpawnParameters);

	return IsValid(SpawnedProjectile) ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
