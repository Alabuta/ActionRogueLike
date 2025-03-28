// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SExplosiveBarrel.generated.h"


class URadialForceComponent;


UCLASS()
class ACTIONROGUELIKE_API ASExplosiveBarrel : public AActor
{
	GENERATED_BODY()

public:

	ASExplosiveBarrel();

protected:

	UPROPERTY(EditAnywhere, Category="Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	TObjectPtr<URadialForceComponent> RadialForceComponent{nullptr};

	UFUNCTION()
	void OnActorHit(
		UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherHitComponent,
		FVector NormalImpulse,
		const FHitResult& Hit);

	virtual void PostInitializeComponents() override;
};
