// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAttributeComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
	FOnHealthChange,
	AActor*, InstigatorActor,
	USAttributeComponent*, OwningComponent,
	float, NewHealth,
	float, Delta);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ACTIONROGUELIKE_API USAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	USAttributeComponent() = default;

	UPROPERTY(BlueprintAssignable)
	FOnHealthChange OnHealthChange;

	UFUNCTION(BlueprintCallable, Category="Attributes")
	bool ApplyHealthChange(AActor* InstigatorActor, float Delta);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Attributes")
	bool IsAlive() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Attributes")
	bool IsFullHealth() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Attributes")
	float GetHealthMax() const;

	UFUNCTION(BlueprintCallable, Category="Attributes")
	static USAttributeComponent* GetAttributeComponent(const AActor* FromActor);

	UFUNCTION(BlueprintCallable, Category="Attributes", meta=(DisplayName="IsAlive"))
	static bool IsActorAlive(const AActor* Actor);

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Config|Attributes")
	float HealthMax = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Config|Attributes")
	float Health = HealthMax;
};
