// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class USInteractionComponent;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class ACTIONROGUELIKE_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, Category="Attack")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, Category="Attack")
	TObjectPtr<UAnimMontage> AttackAnim;

	UPROPERTY(EditAnywhere, Category="Attack")
	FTimerHandle TimerHandle_PrimaryAttack;
	
	UPROPERTY(EditAnywhere, Category="Attack")
	float PrimaryAttackDelay = .2f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USInteractionComponent> InteractionComponent;

	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);

	void PrimaryAttack();
	void PrimaryAttack_TimeElapsed();

	void PrimaryInteract();

public:

	ASCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
};
