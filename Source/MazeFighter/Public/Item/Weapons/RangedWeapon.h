// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Weapons/BaseWeapon.h"
#include "RangedWeapon.generated.h"

/**
 * 
 */

UCLASS()
class MAZEFIGHTER_API ARangedWeapon : public ABaseWeapon
{
	GENERATED_BODY()
	
public:
	ARangedWeapon(const FObjectInitializer& ObjectInitializer);

	bool bAutomatic;
	bool bBurst;

	void FireHitScan();
	void FireShotgun();
};
