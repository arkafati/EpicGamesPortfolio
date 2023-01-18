// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "BaseAttributeSet.generated.h"

// Macros used for attribute set functions
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class MAZEFIGHTER_API UBaseAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	//Function used to update attributes over a network
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	// GAMEPLAY ATTRIBUTES
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Health);

	UPROPERTY(BlueprintReadOnly, Category = "MaxHealth", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, MaxHealth);

	UPROPERTY(BlueprintReadOnly, Category = "Shield", ReplicatedUsing = OnRep_Shield)
	FGameplayAttributeData Shield;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Shield);

	UPROPERTY(BlueprintReadOnly, Category = "MaxShield", ReplicatedUsing = OnRep_MaxShield)
	FGameplayAttributeData MaxShield;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, MaxShield);

	UPROPERTY(BlueprintReadOnly, Category = "AP", ReplicatedUsing = OnRep_AP)
	FGameplayAttributeData AP;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, AP);

	UPROPERTY(BlueprintReadOnly, Category = "MaxAP", ReplicatedUsing = OnRep_MaxAP)
	FGameplayAttributeData MaxAP;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, MaxAP);

	UPROPERTY(BlueprintReadOnly, Category = "Attack", ReplicatedUsing = OnRep_Attack)
	FGameplayAttributeData Attack;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Attack);

	UPROPERTY(BlueprintReadOnly, Category = "MaxAttack", ReplicatedUsing = OnRep_MaxAttack)
	FGameplayAttributeData MaxAttack;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, MaxAttack);

	UPROPERTY(BlueprintReadOnly, Category = "Defense", ReplicatedUsing = OnRep_Defense)
	FGameplayAttributeData Defense;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Defense);

	UPROPERTY(BlueprintReadOnly, Category = "MaxDefense", ReplicatedUsing = OnRep_MaxDefense)
	FGameplayAttributeData MaxDefense;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, MaxDefense);

	UPROPERTY(BlueprintReadOnly, Category = "Speed", ReplicatedUsing = OnRep_Speed)
	FGameplayAttributeData Speed;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Speed);

	UPROPERTY(BlueprintReadOnly, Category = "MaxSpeed", ReplicatedUsing = OnRep_MaxSpeed)
	FGameplayAttributeData MaxSpeed;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, MaxSpeed);

	UPROPERTY(BlueprintReadOnly, Category = "Regen", ReplicatedUsing = OnRep_Regen)
	FGameplayAttributeData Regen;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Regen);

	UPROPERTY(BlueprintReadOnly, Category = "MaxRegen", ReplicatedUsing = OnRep_MaxRegen)
	FGameplayAttributeData MaxRegen;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, MaxRegen);

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Damage)

	//Functions to update and notify attribute changes
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);
	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);
	UFUNCTION()
	virtual void OnRep_Shield(const FGameplayAttributeData& OldShield);
	UFUNCTION()
	virtual void OnRep_MaxShield(const FGameplayAttributeData& OldMaxShield);
	UFUNCTION()
	virtual void OnRep_AP(const FGameplayAttributeData& OldAP);
	UFUNCTION()
	virtual void OnRep_MaxAP(const FGameplayAttributeData& OldMaxAP);
	UFUNCTION()
	virtual void OnRep_Attack(const FGameplayAttributeData& OldAttack);
	UFUNCTION()
	virtual void OnRep_MaxAttack(const FGameplayAttributeData& OldMaxAttack);
	UFUNCTION()
	virtual void OnRep_Defense(const FGameplayAttributeData& OldDefense);
	UFUNCTION()
	virtual void OnRep_MaxDefense(const FGameplayAttributeData& OldMaxDefense);
	UFUNCTION()
	virtual void OnRep_Speed(const FGameplayAttributeData& OldSpeed);
	UFUNCTION()
	virtual void OnRep_MaxSpeed(const FGameplayAttributeData& OldMaxSpeed);
	UFUNCTION()
	virtual void OnRep_Regen(const FGameplayAttributeData& OldRegen);
	UFUNCTION()
	virtual void OnRep_MaxRegen(const FGameplayAttributeData& OldMaxRegen);
};
