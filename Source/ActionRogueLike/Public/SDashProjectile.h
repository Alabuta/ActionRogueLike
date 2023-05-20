﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SProjectileBase.h"
#include "SDashProjectile.generated.h"

UCLASS()
class ACTIONROGUELIKE_API ASDashProjectile : public ASProjectileBase
{
	GENERATED_BODY()

public:

	ASDashProjectile();

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Teleport")
	float TeleportDelay = .2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Teleport")
	float DetonateDelay = .2f;

	FTimerHandle TimerHandle_DelayedDetonate;

	virtual void Explode_Implementation() override;

	virtual void BeginPlay() override;

private:
	
	void TeleportInstigator() const;
};
