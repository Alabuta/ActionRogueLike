// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actions/SAction.h"
#include "Engine/EngineTypes.h"
#include "Templates/SubclassOf.h"
#include "SAction_ProjectileAttack.generated.h"


class ACharacter;
class UParticleSystem;
class UAnimMontage;


/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API USAction_ProjectileAttack : public USAction
{
	GENERATED_BODY()

public:
	
	virtual void StartAction_Implementation(AActor* Instigator) override;

	virtual bool CanStartAction_Implementation(AActor* Instigator) const override;
	
protected:

	UPROPERTY(EditAnywhere, Category="Attack")
	TSubclassOf<AActor> ProjectileClass{nullptr};

	UPROPERTY(EditAnywhere, Category="Attack")
	TObjectPtr<UAnimMontage> AttackAnim{nullptr};

	UPROPERTY(EditDefaultsOnly, Category="FX")
	TObjectPtr<UParticleSystem> CastingVFX{nullptr};

	UPROPERTY(VisibleAnywhere, Category="FX")
	FName MeshSocketName{TEXTVIEW("Muzzle_01")};
	
	UPROPERTY(EditAnywhere, Category="Attack")
	float AttackAnimDelay{.2f};

	UPROPERTY(EditAnywhere, Category="Start Cost", meta=(EditCondition="bCostsRagePoints"))
	float RagePointsCost{50.f};

	UPROPERTY(EditAnywhere, Category="Start Cost", meta=(InlineEditConditionToggle))
	uint8 bCostsRagePoints : 1{false};

private:

	UFUNCTION()
	void AttackDelay_Elapsed(ACharacter* InstigatorCharacter);
};
