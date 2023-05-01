// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGameplayInterface.h"
#include "GameFramework/Actor.h"
#include "SItemChest.generated.h"

UCLASS()
class ACTIONROGUELIKE_API ASItemChest : public AActor, public ISGameplayInterface
{
	GENERATED_BODY()

protected:

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> BaseMeshComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> LidMeshComponent;

	virtual void BeginPlay() override;

public:

	UPROPERTY(EditAnywhere)
	int TargetPitch = 110;

	ASItemChest();

	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

	virtual void Tick(float DeltaTime) override;
};
