// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "SBTService_CheckHealthLevel.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API USBTService_CheckHealthLevel : public UBTService
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, Category="AI")
	FBlackboardKeySelector BlackboardKey;

	UPROPERTY(EditAnywhere, Category="AI", meta=(Units="%", ClampMin="0", ClampMax="100"))
	int32 HealthThresholdLevel{25};

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
