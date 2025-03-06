// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Components/InputComponent.h"


void ASPlayerController::TogglePauseMenu()
{
	if (IsValid(PauseMenuWidget) && PauseMenuWidget->IsInViewport())
	{
		PauseMenuWidget->RemoveFromParent();
		PauseMenuWidget = nullptr;

		bShowMouseCursor = false;
		SetInputMode(FInputModeGameOnly{});
		return;
	}
	
	if (!IsValid(PauseMenuWidgetClass))
	{
		return;
	}

	PauseMenuWidget = CreateWidget<UUserWidget>(this, PauseMenuWidgetClass);
	if (!IsValid(PauseMenuWidget))
	{
		return;
	}

	PauseMenuWidget->AddToViewport(100);
	bShowMouseCursor = true;
	SetInputMode(FInputModeUIOnly{});
}

void ASPlayerController::BeginPlayingState()
{
	Super::BeginPlayingState();

	BlueprintBeginPlayingState();
}

void ASPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction(FName{TEXTVIEW("PauseMenu")}, IE_Pressed, this, &ASPlayerController::TogglePauseMenu);
}

void ASPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	OnPlayerStateChanged.Broadcast(PlayerState);
}
