// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"


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

protected:

	UPROPERTY(EditAnywhere, Category="Config|Primary Attack")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, Category="Config|Primary Attack")
	TObjectPtr<UAnimMontage> AttackAnim;
	
	UPROPERTY(EditAnywhere, Category="Config|Primary Attack")
	float PrimaryAttackDelay = .2f;

	UPROPERTY(EditAnywhere, Category="Config|Black Hole Attack")
	TSubclassOf<AActor> BlackHoleAttackProjectileClass;

	UPROPERTY(EditAnywhere, Category="Config|Black Hole Attack")
	TObjectPtr<UAnimMontage> BlackHoleAttackAnim;
	
	UPROPERTY(EditAnywhere, Category="Config|Black Hole Attack")
	float BlackHoleAttackDelay = .2f;

	UPROPERTY(EditAnywhere, Category="Config|Dash")
	TSubclassOf<AActor> DashProjectileClass;

	UPROPERTY(EditAnywhere, Category="Config|Dash")
	TObjectPtr<UAnimMontage> DashAttackAnim;
	
	UPROPERTY(EditAnywhere, Category="Config|Dash")
	float DashAttackDelay = .2f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USInteractionComponent> InteractionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Config|Components")
	TObjectPtr<USAttributeComponent> AttributeComponent;
	
private:

	FTimerHandle TimerHandle_PrimaryAttack;
	FTimerHandle TimerHandle_BlackHoleAttack;
	FTimerHandle TimerHandle_Dash;

	void MoveForward(float Value);
	void MoveRight(float Value);

	void PrimaryAttack();
	void PrimaryAttack_TimeElapsed();

	void BlackHoleAttack();
	void BlackHoleAttack_TimeElapsed();

	void PrimaryInteract();
	
	void Dash();
	void Dash_TimeElapsed();
	
	void SpawnProjectile(TSubclassOf<AActor> ProjectileClassToSpawn);

	UFUNCTION()
	void OnHealthChanged(
		AActor* InstigatorActor,
		USAttributeComponent* OwningComponent,
		float NewHealth,
		float Delta);

	virtual void PostInitializeComponents() override;
};
