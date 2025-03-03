// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SPlayerState.h"

#include "SSaveGame.h"
#include "Net/UnrealNetwork.h"


int32 ASPlayerState::GetCredits() const
{
	return Credits;
}

void ASPlayerState::AddCredits(const int32 Value)
{
	if (!ensure(Value > 0.f))
	{
		return;
	}

	if (HasAuthority())
	{
		const auto OldCredits = Credits;
		Credits += Value;
		OnRep_Credits(OldCredits);
	}
}

bool ASPlayerState::TryRemoveCredits(const int32 Value)
{
	if (!ensure(Value > 0.f))
	{
		return false;
	}

	if (Credits < Value)
	{
		return false;
	}

	if (HasAuthority())
	{
		const auto OldCredits = Credits;
		Credits -= Value;
		OnRep_Credits(OldCredits);
	}

	return true;
}

void ASPlayerState::SaveState_Implementation(USSaveGame* SaveGameObject)
{
	if (IsValid(SaveGameObject))
	{
		SaveGameObject->Credits = Credits;
	}
}

void ASPlayerState::LoadState_Implementation(const USSaveGame* SaveGameObject)
{
	if (IsValid(SaveGameObject))
	{
		AddCredits(SaveGameObject->Credits);
	}
}

void ASPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Credits);
}

void ASPlayerState::OnRep_Credits(const int32 OldCredits)
{
	OnCreditsChanged.Broadcast(this, Credits, Credits - OldCredits);
}
