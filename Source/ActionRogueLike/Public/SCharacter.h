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

	virtual FVector GetPawnViewLocation() const override;

	UFUNCTION(Exec)
	void HealSelf(const float Amount);

protected:

	UPROPERTY(VisibleAnywhere, Category="Config|FX")
	FName RightHandSocketName{TEXTVIEW("Muzzle_01")};

	UPROPERTY(VisibleAnywhere, Category="Config|FX")
	FName TimeToHitParamName{TEXTVIEW("TimeToHit")};

	UPROPERTY(EditAnywhere, Category="Config|Primary Attack")
	TSubclassOf<AActor> ProjectileClass{nullptr};

	UPROPERTY(EditAnywhere, Category="Config|Primary Attack")
	TObjectPtr<UAnimMontage> AttackAnim{nullptr};
	
	UPROPERTY(EditAnywhere, Category="Config|Primary Attack")
	float PrimaryAttackDelay{.2f};

	UPROPERTY(EditAnywhere, Category="Config|Black Hole Attack")
	TSubclassOf<AActor> BlackHoleAttackProjectileClass{nullptr};

	UPROPERTY(EditAnywhere, Category="Config|Black Hole Attack")
	TObjectPtr<UAnimMontage> BlackHoleAttackAnim{nullptr};
	
	UPROPERTY(EditAnywhere, Category="Config|Black Hole Attack")
	float BlackHoleAttackDelay{.2f};

	UPROPERTY(EditAnywhere, Category="Config|Dash")
	TSubclassOf<AActor> DashProjectileClass{nullptr};

	UPROPERTY(EditAnywhere, Category="Config|Dash")
	TObjectPtr<UAnimMontage> DashAttackAnim{nullptr};
	
	UPROPERTY(EditAnywhere, Category="Config|Dash")
	float DashAttackDelay{.2f};

	UPROPERTY(VisibleAnywhere, Category="Config|Components")
	TObjectPtr<USpringArmComponent> SpringArmComponent{nullptr};

	UPROPERTY(VisibleAnywhere, Category="Config|Components")
	TObjectPtr<UCameraComponent> CameraComponent{nullptr};

	UPROPERTY(VisibleAnywhere, Category="Config|Components")
	TObjectPtr<USInteractionComponent> InteractionComponent{nullptr};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Config|Components")
	TObjectPtr<USAttributeComponent> AttributeComponent{nullptr};

	UPROPERTY(EditDefaultsOnly, Category="Config|FX")
	TObjectPtr<UParticleSystem> CastingVfx{nullptr};
	
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

	void StartAttackFXs(UAnimMontage* AttackAnimMontage);
	
	void SpawnProjectile(TSubclassOf<AActor> ProjectileClassToSpawn);

	UFUNCTION()
	void OnHealthChanged(
		AActor* InstigatorActor,
		USAttributeComponent* OwningComponent,
		const float NewHealth,
		const float Delta);

	virtual void PostInitializeComponents() override;
};
