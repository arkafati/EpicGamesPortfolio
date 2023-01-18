// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class MazeFAbilityID : uint8
{
	None			UMETA(DisplayName = "None"),
	Cancel			UMETA(DisplayName = "Cancel"),
	Confirm			UMETA(DisplayName = "Confirm"),
	IA_Jump
};
