// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTTask_RangedAttack.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"


EBTNodeResult::Type USBTTask_RangedAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (const auto* AIController = OwnerComp.GetAIOwner(); AIController != nullptr)
	{
		const auto* Pawn = Cast<ACharacter>(AIController->GetPawn());
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

			const auto PawnMuzzleLocation = Pawn->GetMesh()->GetSocketLocation(TEXT("Muzzle_01"));
			const auto Direction = TargetActor->GetActorLocation() - PawnMuzzleLocation;
			const auto PawnMuzzleRotation = Direction.Rotation();

			FActorSpawnParameters SpawnParameters;
			SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

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
