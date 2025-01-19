// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTTask_RangedAttack.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SAttributeComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"


EBTNodeResult::Type USBTTask_RangedAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (const auto* AIController = OwnerComp.GetAIOwner(); AIController != nullptr)
	{
		auto* Pawn = Cast<ACharacter>(AIController->GetPawn());
		if (Pawn == nullptr)
		{
			return EBTNodeResult::Failed;
		}

		const auto* BlackboardComponent = OwnerComp.GetBlackboardComponent();
		if (ensure(BlackboardComponent != nullptr))
		{
			const auto* TargetActor = Cast<AActor>(BlackboardComponent->GetValueAsObject(TEXT("TargetActor")));
			if (TargetActor == nullptr)
			{
				return EBTNodeResult::Failed;
			}

			if (const auto IsTargetActorAlive = USAttributeComponent::IsActorAlive(TargetActor); !IsTargetActorAlive)
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

			return SpawnedProjectile != nullptr ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
		}
	}
	return EBTNodeResult::Failed;
}
