// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SAnimInstance.generated.h"

class USActionComponent;
/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API USAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:

	UPROPERTY(Transient, BlueprintReadOnly, Category="Components")
	TObjectPtr<USActionComponent> ActionComponent{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="State")
	uint8 bStunned : 1{false};
};
