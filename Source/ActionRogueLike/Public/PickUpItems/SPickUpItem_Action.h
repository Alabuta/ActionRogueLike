// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickUpItems/SPickUpItemBase.h"
#include "SPickUpItem_Action.generated.h"

class USAction;
/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASPickUpItem_Action : public ASPickUpItemBase
{
	GENERATED_BODY()

public:

	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

protected:

	UPROPERTY(EditAnywhere, Category="Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent{nullptr};

	UPROPERTY(EditAnywhere)
	TSubclassOf<USAction> ActionClassToGrant;
};
