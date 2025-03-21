// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Stats/Stats2.h"


DECLARE_STATS_GROUP(TEXT("STANFORD_Game"), STATGROUP_STANFORD, STATCAT_Advanced);


void LogOnScreen(
	const UObject* WorldContext,
	const FString& Message,
	const FColor Color = FColor::White,
	const float Duration = 5.f);
