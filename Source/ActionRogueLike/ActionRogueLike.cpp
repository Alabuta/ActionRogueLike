// Copyright Epic Games, Inc. All Rights Reserved.

#include "ActionRogueLike.h"
#include "Modules/ModuleManager.h"
#include "Engine/Engine.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, ActionRogueLike, "ActionRogueLike" );

void LogOnScreen(
	const UObject* WorldContext,
	const FString& Message,
	const FColor Color,
	const float Duration)
{
	if (!ensure(IsValid(WorldContext)))
	{
		return;
	}

	const auto* World = WorldContext->GetWorld();
	if (!ensure(IsValid(World)))
	{
		return;
	}

	if (IsValid(GEngine))
	{
		const FString NetPrefix{World->IsNetMode(NM_Client) ? TEXT("[CLIENT] : ") : TEXT("[SERVER] : ")};
		GEngine->AddOnScreenDebugMessage(-1, Duration, Color, NetPrefix + Message);
	}
}

