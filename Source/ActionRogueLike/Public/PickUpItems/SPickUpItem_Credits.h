// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickUpItems/SPickUpItemBase.h"
#include "SPickUpItem_Credits.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASPickUpItem_Credits : public ASPickUpItemBase
{
	GENERATED_BODY()

public:

	ASPickUpItem_Credits();

	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

protected:

	UPROPERTY(EditAnywhere, Category="Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent{nullptr};

	UPROPERTY(EditAnywhere)
	float CreditsAmount{80.f};
};
