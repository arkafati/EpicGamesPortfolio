// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MAZEFIGHTER_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInventoryComponent();
	friend class APlayerCharacter;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
	TArray<class ABaseWeapon*> WeaponInv;

	class APlayerCharacter* Character;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
	void ServerAddToInventory(class AItem* Weapon);
	virtual void AddToInventory(class AItem* Weapon);

	virtual void WeaponAdd(class ABaseWeapon* Weapon);

	virtual void InInventoryHandle(class ABaseWeapon* Weapon);
	virtual void SwapIndex(int32 IndexOne, int32 IndexToSwap);

	virtual int32 IndexOfWeaponAdded(int32 Index);

	int32 GetCurrentWeaponIndex();

	UFUNCTION(Server, Reliable)
	void ServerEquipWeaponOne();
	virtual void EquipWeaponOne();

	UFUNCTION(Server, Reliable)
	void ServerEquipWeaponTwo();
	virtual void EquipWeaponTwo();

	UFUNCTION(Server, Reliable)
	void ServerEquipWeaponThree();
	virtual void EquipWeaponThree();

	virtual void EquipPreviousWeapon();
	virtual void EquipNextWeapon();

	UPROPERTY(VisibleAnywhere)
	int32 InvIndex;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void DisplayInventory();
	void StopShowingInventory();

};