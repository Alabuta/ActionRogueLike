// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SAICharacter.generated.h"


class USAttributeComponent;
class UPawnSensingComponent;

UCLASS()
class ACTIONROGUELIKE_API ASAICharacter : public ACharacter
{
	GENERATED_BODY()

public:

	ASAICharacter();

	virtual void PostInitializeComponents() override;

protected:

	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<UPawnSensingComponent> PawnSensingComponent;

	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<USAttributeComponent> AttributeComponent;

	UPROPERTY(VisibleAnywhere, Category="Config|FX")
	FName TimeToHitParamName{TEXT("TimeToHit")};

	UFUNCTION()
	void OnPawnSeen(APawn* Pawn);

	UFUNCTION()
	void OnHealthChanged(
		AActor* InstigatorActor,
		USAttributeComponent* OwningComponent,
		float NewHealth,
		float Delta);

	bool SetNewTarget(AActor* NewTargetActor) const;
};
