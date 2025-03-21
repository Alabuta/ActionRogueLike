﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAttributeComponent.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
	FOnAttributeChange,
	AActor*, InstigatorActor,
	USAttributeComponent*, OwningComponent,
	const float, NewValue,
	const float, Delta);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ACTIONROGUELIKE_API USAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	USAttributeComponent();

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChange OnHealthChange;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChange OnRageChange;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Attributes")
	bool IsAlive() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Attributes")
	bool IsFullHealth() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Attributes")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Attributes")
	float GetHealthMax() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Attributes")
	float GetHealthRatio() const;

	UFUNCTION(BlueprintCallable, Category="Attributes")
	bool ApplyHealthChange(AActor* InstigatorActor, float Delta);

	UFUNCTION(BlueprintCallable, Category="Attributes")
	void Kill(AActor* InstigatorActor);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Attributes")
	float GetRage() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Attributes")
	float GetRageMax() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Attributes")
	float GetRageRatio() const;

	UFUNCTION(BlueprintCallable, Category="Attributes")
	bool ApplyRageChange(AActor* InstigatorActor, float Delta);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastHealthChange(AActor* InstigatorActor, const float NewValue, const float Delta);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRageChange(AActor* InstigatorActor, const float NewValue, const float Delta);

	UFUNCTION(BlueprintCallable, Category="Attributes")
	static USAttributeComponent* GetAttributeComponent(const AActor* FromActor);

	UFUNCTION(BlueprintCallable, Category="Attributes", meta=(DisplayName="IsAlive"))
	static bool IsActorAlive(const AActor* Actor);

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category="Attributes|Health")
	float Health{0.f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category="Attributes|Health")
	float HealthMax{100.f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category="Attributes|Rage")
	float Rage{0.f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category="Attributes|Rage")
	float RageMax{100.f};
};
