// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTService_CheckAttackRange.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"


void USBTService_CheckAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	auto* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!ensure(BlackboardComponent != nullptr))
	{
		return;
	}
	// OwnerComp.CacheBlackboardComponent(BlackboardComponent);

	const auto* TargetActor = Cast<AActor>(BlackboardComponent->GetValueAsObject(TEXT("TargetActor")));
	if (!IsValid(TargetActor))
	{
		return;
	}

	const auto* AIController = OwnerComp.GetAIOwner(); 
	if (!ensure(IsValid(AIController)))
	{
		return;
	}

	const auto* AIPawn = AIController->GetPawn(); 
	if (!ensure(IsValid(AIPawn)))
	{
		return;
	}

	bool bHasLineOfSight = false;

	const auto DistanceTo = FVector::DistSquared(TargetActor->GetActorLocation(), AIPawn->GetActorLocation());
	const auto bWithinAttackRange = DistanceTo <= FMath::Square(AttackRange);
	if (bWithinAttackRange)
	{
		bHasLineOfSight = AIController->LineOfSightTo(TargetActor);
	}

	BlackboardComponent->SetValueAsBool(AttackRangeKey.SelectedKeyName, bWithinAttackRange && bHasLineOfSight);
}
