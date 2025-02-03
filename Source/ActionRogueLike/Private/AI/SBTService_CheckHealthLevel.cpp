// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTService_CheckHealthLevel.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SAttributeComponent.h"


void USBTService_CheckHealthLevel::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	auto* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!ensure(BlackboardComponent != nullptr))
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

	const auto* AIPawnAttributeComponent = USAttributeComponent::GetAttributeComponent(AIPawn);
	if (!ensure(IsValid(AIPawnAttributeComponent)))
	{
		return;
	}

	const auto bIsHealthBelowThreshold =
		FMath::CeilToInt(AIPawnAttributeComponent->GetHealthRatio() * 100.f) < HealthThresholdLevel;
	BlackboardComponent->SetValueAsBool(BlackboardKey.SelectedKeyName, bIsHealthBelowThreshold);
}
