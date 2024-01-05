// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTService_CheckAttackRange.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"


void USBTService_CheckAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	auto* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (ensure(BlackboardComponent != nullptr))
	{
		// OwnerComp.CacheBlackboardComponent(BlackboardComponent);

		const auto* TargetActor = Cast<AActor>(BlackboardComponent->GetValueAsObject(TEXT("TargetActor")));
		if (TargetActor != nullptr)
		{
			if (const auto* AIController = OwnerComp.GetAIOwner(); ensure(AIController != nullptr))
			{
				if (const auto* AIPawn = AIController->GetPawn(); ensure(AIPawn != nullptr))
				{
					const auto DistanceTo = FVector::DistSquared(
						TargetActor->GetActorLocation(),
						AIPawn->GetActorLocation());

					const auto bWithinAttackRange = DistanceTo < 1'500. * 1'500.;
					bool bHasLineOfSight = false;
					if (bWithinAttackRange)
					{
						bHasLineOfSight = AIController->LineOfSightTo(TargetActor);
					}

					BlackboardComponent->SetValueAsBool(AttackRangeKey.SelectedKeyName, bWithinAttackRange && bHasLineOfSight);
				}
			}
		}
	}
}
