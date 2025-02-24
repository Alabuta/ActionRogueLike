// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SPlayerState.h"


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

	Credits += Value;
	OnCreditsChanged.Broadcast(this, Credits, Value);
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

	Credits -= Value;
	OnCreditsChanged.Broadcast(this, Credits, -Value);

	return true;
}
