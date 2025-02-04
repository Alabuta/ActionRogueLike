// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTTask_Heal.h"

#include "AIController.h"
#include "Components/SAttributeComponent.h"
#include "GameFramework/Character.h"


EBTNodeResult::Type USBTTask_Heal::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const auto* AIController = OwnerComp.GetAIOwner();
	if (!IsValid(AIController))
	{
		return EBTNodeResult::Failed;
	}

	auto* AIPawn = AIController->GetPawn();
	if (!IsValid(AIPawn))
	{
		return EBTNodeResult::Failed;
	}

	auto* AIPawnAttributeComponent = USAttributeComponent::GetAttributeComponent(AIPawn);
	if (!ensure(IsValid(AIPawnAttributeComponent)))
	{
		return EBTNodeResult::Failed;
	}

	if (!AIPawnAttributeComponent->IsAlive())
	{
		return EBTNodeResult::Aborted;
	}

	float Delta;

	if (bHealToTargetValue)
	{
		const auto TargetLevel = static_cast<float>(
			FMath::CeilToInt(AIPawnAttributeComponent->GetHealthMax() * static_cast<float>(HealValuePercentage) * .01f));
		Delta = TargetLevel - AIPawnAttributeComponent->GetHealth();
	}
	else
	{
		Delta = static_cast<float>(
			FMath::CeilToInt(AIPawnAttributeComponent->GetHealthMax() * static_cast<float>(HealValuePercentage) * .01f));
	}

	if (FMath::IsNearlyZero(Delta, .1f))
	{
		return EBTNodeResult::Aborted;
	}

	return AIPawnAttributeComponent->ApplyHealthChange(AIPawn, Delta)
		       ? EBTNodeResult::Succeeded
		       : EBTNodeResult::Aborted;
}
