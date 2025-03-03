// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SAICharacter.generated.h"


class USActionComponent;
class USWorldUserWidget;
class USAttributeComponent;
class UPawnSensingComponent;

UCLASS()
class ACTIONROGUELIKE_API ASAICharacter : public ACharacter
{
	GENERATED_BODY()

public:

	ASAICharacter();

	virtual void PostInitializeComponents() override;

protected:

	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<UPawnSensingComponent> PawnSensingComponent{nullptr};

	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<USAttributeComponent> AttributeComponent{nullptr};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USActionComponent> ActionComponent{nullptr};

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<USWorldUserWidget> HealthBarWidgetClass{nullptr};

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<USWorldUserWidget> SpottedWidgetClass{nullptr};

	UPROPERTY(VisibleAnywhere, Category="FX")
	FName TimeToHitParamName{TEXTVIEW("TimeToHit")};

	UFUNCTION(BlueprintCallable)
	AActor* GetTargetActor() const;

	UFUNCTION()
	void OnPawnSeen(APawn* Pawn);

	UFUNCTION()
	void OnHealthChanged(
		AActor* InstigatorActor,
		USAttributeComponent* OwningComponent,
		const float NewValue,
		const float Delta);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPawnSeen();

	bool SetTargetActor(AActor* NewTargetActor) const;

private:

	UPROPERTY(Transient)
	USWorldUserWidget* ActiveHealthBarWidget{nullptr};
};
