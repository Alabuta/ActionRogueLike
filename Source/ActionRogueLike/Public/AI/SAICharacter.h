// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SAICharacter.generated.h"


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

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<USWorldUserWidget> HealthBarWidgetClass{nullptr};

	UPROPERTY(VisibleAnywhere, Category="Config|FX")
	FName TimeToHitParamName{TEXTVIEW("TimeToHit")};

	UFUNCTION()
	void OnPawnSeen(APawn* Pawn);

	UFUNCTION()
	void OnHealthChanged(
		AActor* InstigatorActor,
		USAttributeComponent* OwningComponent,
		const float NewHealth,
		const float Delta);

	bool SetActorTarget(AActor* NewTargetActor) const;

private:

	UPROPERTY(Transient)
	USWorldUserWidget* ActiveHealthBarWidget{nullptr};
};
