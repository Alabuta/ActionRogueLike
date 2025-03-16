// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/NoExportTypes.h"
#include "SAction.generated.h"


class UTexture2D;
class USActionComponent;


USTRUCT()
struct FActionRepData
{
	GENERATED_BODY()

	UPROPERTY()
	AActor* Instigator{nullptr};

	UPROPERTY()
	uint8 bIsRunning : 1{false};
};


/**
 * 
 */
UCLASS(Blueprintable)
class ACTIONROGUELIKE_API USAction : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category="Action")
	FName ActionName{NAME_None};

	UPROPERTY(EditDefaultsOnly, Category="Action")
	uint8 bAutoStart : 1{false};

	void Initialize(USActionComponent* NewActionComponent);

	UFUNCTION(BlueprintNativeEvent, Category="Action")
	bool CanStartAction(AActor* Instigator) const;

	UFUNCTION(BlueprintNativeEvent, Category="Action")
	bool IsRunning() const;

	UFUNCTION(BlueprintNativeEvent, Category="Action")
	void StartAction(AActor* Instigator);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Action")
	void StopAction(AActor* Instigator);

	virtual UWorld* GetWorld() const override;

	virtual bool IsSupportedForNetworking() const override;

protected:

	UPROPERTY(EditDefaultsOnly, Category="Tags")
	FGameplayTagContainer GrantsTags;

	UPROPERTY(EditDefaultsOnly, Category="Tags")
	FGameplayTagContainer BlockedTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI")
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(Transient, Replicated)
	float TimeStarted{0.f};

	UFUNCTION(BlueprintCallable, Category="Action")
	USActionComponent* GetOwningComponent() const;

private:

	UPROPERTY(Transient, Replicated)
	USActionComponent* OwningActionComponent;

	UPROPERTY(ReplicatedUsing="OnRep_RepData")
	FActionRepData RepData;

	UFUNCTION()
	void OnRep_RepData();
};

inline bool USAction::IsSupportedForNetworking() const
{
	return true;
}
