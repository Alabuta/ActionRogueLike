// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"


class USActionComponent;
class USAttributeComponent;
class USInteractionComponent;
class UCameraComponent;
class USpringArmComponent;


UCLASS()
class ACTIONROGUELIKE_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	ASCharacter();

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual FVector GetPawnViewLocation() const override;

	UFUNCTION(Exec)
	void HealSelf(const float Amount);

protected:

	UPROPERTY(VisibleAnywhere, Category="Config|FX")
	FName TimeToHitParamName{TEXTVIEW("TimeToHit")};

	UPROPERTY(VisibleAnywhere, Category="Config|Components")
	TObjectPtr<USpringArmComponent> SpringArmComponent{nullptr};

	UPROPERTY(VisibleAnywhere, Category="Config|Components")
	TObjectPtr<UCameraComponent> CameraComponent{nullptr};

	UPROPERTY(VisibleAnywhere, Category="Config|Components")
	TObjectPtr<USInteractionComponent> InteractionComponent{nullptr};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Config|Components")
	TObjectPtr<USAttributeComponent> AttributeComponent{nullptr};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Config|Components")
	TObjectPtr<USActionComponent> ActionComponent{nullptr};

private:

	virtual void PostInitializeComponents() override;

	void MoveForward(float Value);
	void MoveRight(float Value);

	void PrimaryAttack();
	void BlackHoleAttack();

	void Dash();

	void PrimaryInteract();

	void SprintStart();
	void SprintStop();

	UFUNCTION()
	void OnHealthChanged(
		AActor* InstigatorActor,
		USAttributeComponent* OwningComponent,
		const float NewHealth,
		const float Delta);
};
