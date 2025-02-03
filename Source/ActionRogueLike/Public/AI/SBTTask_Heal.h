// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SBTTask_Heal.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API USBTTask_Heal : public UBTTaskNode
{
	GENERATED_BODY()

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:

	UPROPERTY(EditAnywhere, Category="AI", meta=(Units="%", ClampMin="0", ClampMax="100", EditCondition="bHealToTargetValue"))
	int32 TargetHealthPercentage{100};

	UPROPERTY(EditAnywhere, Category="AI", meta=(Units="%", ClampMin="0", ClampMax="100", EditCondition="!bHealToTargetValue"))
	int32 HealAmountPercentage{25};

	// Flag that controls whether health should be healed to specific target level or increased by specific value
	UPROPERTY(EditAnywhere, Category="AI")
	uint32 bHealToTargetValue : 1{true};
};
