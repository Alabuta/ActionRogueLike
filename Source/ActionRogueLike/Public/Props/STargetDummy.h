﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STargetDummy.generated.h"


class USAttributeComponent;


UCLASS()
class ACTIONROGUELIKE_API ASTargetDummy : public AActor
{
	GENERATED_BODY()

public:

	ASTargetDummy();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USAttributeComponent> AttributeComponent{nullptr};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent{nullptr};

	UFUNCTION()
	void OnHealthChanged(
		AActor* InstigatorActor,
		USAttributeComponent* OwningComponent,
		const float NewHealth,
		const float Delta);
};
