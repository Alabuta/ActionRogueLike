// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SProjectileBase.generated.h"


class UProjectileMovementComponent;
class USphereComponent;


UCLASS(Abstract)
class ACTIONROGUELIKE_API ASProjectileBase : public AActor
{
	GENERATED_BODY()

public:

	ASProjectileBase();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USphereComponent> ColliderComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UProjectileMovementComponent> MovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UParticleSystemComponent> VfxComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UAudioComponent> FlightAudioComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="FX")
	TObjectPtr<USoundBase> ImpactSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="FX")
	TObjectPtr<UParticleSystem> ImpactVfx;

	UPROPERTY(EditDefaultsOnly, Category = "FX|Camera Shake")
	float ImpactCameraShakeInnerRadius = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "FX|Camera Shake")
	float ImpactCameraShakeOuterRadius = 1500.f;

	UPROPERTY(EditDefaultsOnly, Category = "FX|Camera Shake")
	TSubclassOf<UCameraShakeBase> ImpactCameraShake;

	virtual void PostInitializeComponents() override;

	UFUNCTION()
	virtual void OnActorHit(
		UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		FVector NormalImpulse,
		const FHitResult& Hit);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Explode();

	void ApplyHitFX() const;
};
