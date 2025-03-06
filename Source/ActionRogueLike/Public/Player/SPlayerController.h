// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerController.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStateChanged, APlayerState*, NewPlayerState);

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> PauseMenuWidgetClass;

	UPROPERTY()
	FOnPlayerStateChanged OnPlayerStateChanged;

	UFUNCTION(BlueprintImplementableEvent)
	void BlueprintBeginPlayingState();

	UFUNCTION(BlueprintCallable, Category="UI")
	void TogglePauseMenu();

	virtual void BeginPlayingState() override;

	virtual void SetupInputComponent() override;

	virtual void OnRep_PlayerState() override;

private:

	UPROPERTY(Transient)
	UUserWidget* PauseMenuWidget;
};
