// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include <MazeFighter/MazeFighter.h>
#include "GameplayAbilityCore.generated.h"

/**
 * 
 */
UCLASS()
class MAZEFIGHTER_API UGameplayAbilityCore : public UGameplayAbility
{
	GENERATED_BODY()
	
public: 
	UGameplayAbilityCore();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
	MazeFAbilityID AbilityInputID = MazeFAbilityID::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
	MazeFAbilityID AbilityID = MazeFAbilityID::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	bool ActivateAbilityOnGranted = false;

	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

};
