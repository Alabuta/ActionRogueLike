// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGameplayInterface.h"
#include "GameFramework/Actor.h"
#include "SPickUpItemBase.generated.h"


class USphereComponent;


UCLASS(Abstract)
class ACTIONROGUELIKE_API ASPickUpItemBase : public AActor, public ISGameplayInterface
{
	GENERATED_BODY()

public:

	ASPickUpItemBase();

	virtual void Interact_Implementation(APawn* InstigatorPawn) override
	PURE_VIRTUAL(ISGameplayInterface::Interact, ensureMsgf(false && "NotImplemented", TEXT("NotImplemented")););

protected:

	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<USphereComponent> ColliderComponent{nullptr};

	UPROPERTY(EditAnywhere, Category="")
	float RespawnTime{10.f};

	UPROPERTY(ReplicatedUsing="OnRep_IsActive")
	bool bIsActive{false};

	FTimerHandle TimerHandle_RespawnTimer;

	UFUNCTION()
	void OnRep_IsActive();

	UFUNCTION()
	void Show();
	void HideAndCooldown();

	void SetState(const bool bNewIsActive);
};
