// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"


void LogOnScreen(
	const UObject* WorldContext,
	const FString& Message,
	const FColor Color = FColor::White,
	const float Duration = 5.f);
