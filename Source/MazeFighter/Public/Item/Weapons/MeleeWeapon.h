// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Weapons/BaseWeapon.h"
#include "MeleeWeapon.generated.h"

/**
 * 
 */
UCLASS()
class MAZEFIGHTER_API AMeleeWeapon : public ABaseWeapon
{
	GENERATED_BODY()
	
public:
	AMeleeWeapon(const FObjectInitializer& ObjectInitializer);

};


