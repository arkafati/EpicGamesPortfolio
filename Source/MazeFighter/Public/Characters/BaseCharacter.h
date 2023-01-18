// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include <MazeFighter/MazeFighter.h>
#include "GameplayTagContainer.h"
#include "BaseCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterDiedDelegate, ABaseCharacter*, Character);

UCLASS()
class MAZEFIGHTER_API ABaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter(const class FObjectInitializer& ObjectInitializer);

	//Actor Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UInventoryComponent* InventoryComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCombatComponent* CombatComp;

	//Handles the Replication of Objects
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//Getter for Character Ability System Component
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// Tells other players when we have died
	UPROPERTY(BlueprintAssignable, Category = "Character")
	FCharacterDiedDelegate OnCharacterDied;

	// Check to see if we are alive
	UFUNCTION(BlueprintCallable)
	virtual bool IsAlive() const;

	// Handle Death or Remove Abilities
	virtual void RemoveCharacterAbilities();
	virtual void Die();
	UFUNCTION(BlueprintCallable, Category = "Character")
	virtual void FinishDying();

	// Getter for Ability Level -- Needed for execution of ability (For now all abilities in this project will be 1)
	UFUNCTION(BlueprintCallable)
	virtual int32 GetAbilityLevel(MazeFAbilityID AbilityID) const;

	// Getters for Attributes
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetHealth() const;
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetMaxHealth() const;
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetShield() const;
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetMaxShield() const;
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetAP() const;
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetMaxAP() const;
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetAttack() const;
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetMaxAttack() const;
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetDefense() const;
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetMaxDefense() const;
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetSpeed() const;
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetMaxSpeed() const;
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetRegen() const;
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetMaxRegen() const;

	//A line trace is performed every frame, these variables will give us more info on what we hit
	UPROPERTY(ReplicatedUsing = OnRep_LastItemHit, VisibleAnywhere)
	class AItem* ItemHitLastFrame;
	UPROPERTY(ReplicatedUsing = OnRep_ShouldTrace, VisibleAnywhere)
	bool bShouldTraceItem;

	//Check to see if we are in range to pickup items and what items to pickup
	UFUNCTION()
	virtual void OnRep_ShouldTrace();
	UFUNCTION()
	virtual void OnRep_LastItemHit(class AItem* Item);

	//Line trace that is performed every frame and line trace to pickup items
	virtual void TraceUnderCrosshair(FHitResult& OutHitResult);
	virtual void ItemTrace(FHitResult& OutHitResult);

	// Adds and subtracts items to an array to let us know what we can and cant pickup
	virtual void AddItemToPickup(class AItem* Item);
	virtual void SubtractItemToPickup(class AItem* Item);

	//Returns whether or not there is an equipped weapon and what weapon is equipped
	class ABaseWeapon* GetEquippedWeapon();
	bool IsWeaponEquipped();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Store the Ability System and Attribute Set in a less expensive way than a pointer
	TWeakObjectPtr<class UCharacterASC> AbilitySystemComponent;
	TWeakObjectPtr<class UBaseAttributeSet> AttributeSetBase;

	// GAMEPLAY TAGS
	FGameplayTag DeadTag;
	FGameplayTag EffectRemoveOnDeathTag;

	//Death Animation Montage
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* DeathMontage;

	//Attributes and Effects to apply to character on spawn
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
	TSubclassOf<class UGameplayEffect> DefaultAttributes;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
	TArray<TSubclassOf<class UGameplayEffect>> StartupEffects;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
	TArray < TSubclassOf<class UGameplayAbilityCore>> CharacterAbilities;

	//Add Abilities and Effects
	virtual void AddCharacterAbilities();
	virtual void InitializeAttributes();
	virtual void AddStartupEffects();

	//Manually set Attributes - Only recommended to do on spawn
	virtual void SetHealth(float Health);
	virtual void SetShield(float Shield);
	virtual void SetAP(float AP);

	//Array of Items we can pickup
	UPROPERTY(Replicated, VisibleAnywhere)
	TArray<class AItem*> ItemsToPickup;

};
