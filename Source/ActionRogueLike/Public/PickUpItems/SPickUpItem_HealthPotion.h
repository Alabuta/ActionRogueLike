// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickUpItems/SPickUpItemBase.h"
#include "SPickUpItem_HealthPotion.generated.h"


UCLASS()
class ACTIONROGUELIKE_API ASPickUpItem_HealthPotion : public ASPickUpItemBase
{
	GENERATED_BODY()

public:

	ASPickUpItem_HealthPotion();

	virtual FText GetInteractText_Implementation(APawn* InstigatorPawn) override;
	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

protected:

	UPROPERTY(EditAnywhere, Category="Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent{nullptr};

	UPROPERTY(EditAnywhere)
	float CreditCost{50.f};
};
