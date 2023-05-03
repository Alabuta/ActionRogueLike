// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SMagicProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class ACTIONROGUELIKE_API ASMagicProjectile : public AActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TObjectPtr<USphereComponent> SphereComponent;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TObjectPtr<UProjectileMovementComponent> MovementComponent;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TObjectPtr<UParticleSystemComponent> FXComponent;

	virtual void BeginPlay() override;

public:

	ASMagicProjectile();

	virtual void Tick(float DeltaTime) override;
};
