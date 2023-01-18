// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h"
#include "BasePlayerState.generated.h"

/**
 * 
 */
UCLASS()
class MAZEFIGHTER_API ABasePlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	ABasePlayerState();

	class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	class UBaseAttributeSet* GetAttributeSetBase() const;

	UFUNCTION()
	bool IsAlive() const;

	UFUNCTION()
	float GetHealth() const;
	UFUNCTION()
	float GetMaxHealth() const;
	UFUNCTION()
	float GetShield() const;
	UFUNCTION()
	float GetMaxShield() const;
	UFUNCTION()
	float GetAP() const;
	UFUNCTION()
	float GetMaxAP() const;
	UFUNCTION()
	float GetAttack() const;
	UFUNCTION()
	float GetMaxAttack() const;
	UFUNCTION()
	float GetDefense() const;
	UFUNCTION()
	float GetMaxDefense() const;
	UFUNCTION()
	float GetSpeed() const;
	UFUNCTION()
	float GetMaxSpeed() const;
	UFUNCTION()
	float GetRegen() const;
	UFUNCTION()
	float GetMaxRegen() const;

protected:
	UPROPERTY()
	class UCharacterASC* AbilitySystemComponent;
	UPROPERTY()
	class UBaseAttributeSet* AttributeSetBase;

	FGameplayTag DeadTag;

	FDelegateHandle HealthChangedDelegateHandle;

	virtual void BeginPlay() override;

	virtual void HealthChanged(const FOnAttributeChangeData& Data);
};
