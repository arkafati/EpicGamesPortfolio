// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "CharacterASC.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FReceivedDamageDelegate, UCharacterASC*, SourceASC, float, UnmitigatedDamage, float, MitigatedDamage);

/**
 * 
 */
UCLASS()
class MAZEFIGHTER_API UCharacterASC : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	bool CharacterAbilitiesGiven = false;
	bool StartupEffectsApplied = false;
	bool AttributesInitialized = false;

	FReceivedDamageDelegate ReceivedDamage;

	virtual void ReceiveDamage(UCharacterASC* SourceASC, float UnmitigatedDamage, float MitigatedDamage);
};
