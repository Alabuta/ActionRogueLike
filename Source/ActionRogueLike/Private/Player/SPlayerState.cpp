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
		Credits += Value;
		MulticastCreditsChange_Implementation(this, Credits, Value);
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
		Credits -= Value;
		MulticastCreditsChange_Implementation(this, Credits, -Value);
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
		Credits = SaveGameObject->Credits;
	}
}

void ASPlayerState::MulticastCreditsChange_Implementation(
	AActor* InstigatorActor,
	const int32 NewValue,
	const int32 Delta)
{
	OnCreditsChanged.Broadcast(this, Credits, Delta);
}

void ASPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Credits);
}
