// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Components/InventoryComponent.h"
#include "Characters/Player/PlayerCharacter.h"
#include "Item/Weapons/MeleeWeapon.h"
#include "Item/Weapons/RangedWeapon.h"
#include "Blueprint/UserWidget.h"
#include "Net/UnrealNetwork.h"
#include "Item/Weapons/BaseWeapon.h"
#include "Characters/Components/CombatComponent.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(true);

}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, WeaponInv);

}

void UInventoryComponent::ServerAddToInventory_Implementation(AItem* Weapon)
{
	AddToInventory(Weapon);
}

void UInventoryComponent::AddToInventory(AItem* Weapon)
{
	if (Character->HasAuthority())
	{
		if (Weapon)
		{
			ABaseWeapon* BW = Cast<ABaseWeapon>(Weapon);
			if (BW)
			{
				WeaponAdd(BW);
			}

		}
		return;
	}
	else
	{
		ServerAddToInventory(Weapon);
	}
}

void UInventoryComponent::WeaponAdd(ABaseWeapon* Weapon)
{
	for (int32 i = 0; i < WeaponInv.Num(); i++)
	{
		if (WeaponInv[i] == nullptr)
		{
			WeaponInv.Insert(Weapon, i);
			WeaponInv.RemoveAt(IndexOfWeaponAdded(i + 1));
			if (Character->CombatComp->EquippedWeapon == nullptr)
			{
				Character->CombatComp->AttachToPawn(Weapon);
				Character->CombatComp->Equip(Weapon);
				return;
			}
			else
			{
				InInventoryHandle(Weapon);
				return;
			}
		}
	}

	if (Character->CombatComp->EquippedWeapon && WeaponInv.Contains(Character->CombatComp->EquippedWeapon))
	{
		
		Character->CombatComp->DropWeapon(Character->CombatComp->EquippedWeapon);
		WeaponInv.Insert(Weapon, InvIndex);
		WeaponInv.RemoveAt(InvIndex + 1);
		Character->CombatComp->AttachToPawn(Weapon);
		Character->CombatComp->Equip(Weapon);
		return;
	}

}

int32 UInventoryComponent::GetCurrentWeaponIndex()
{
	if (InvIndex)
	{
		return InvIndex;
	}
	return 0;
}

void UInventoryComponent::ServerEquipWeaponOne_Implementation()
{
	EquipWeaponOne();
}

void UInventoryComponent::EquipWeaponOne()
{
	if (Character->HasAuthority())
	{
		ABaseWeapon* BW = Cast<ABaseWeapon>(WeaponInv[0]);
		if (BW)
		{
			Character->CombatComp->Equip(BW);
			InvIndex = 0;
		}
	}
	else
	{
		ServerEquipWeaponOne();
	}
}

void UInventoryComponent::ServerEquipWeaponTwo_Implementation()
{
	EquipWeaponTwo();
}

void UInventoryComponent::EquipWeaponTwo()
{
	if (Character->HasAuthority())
	{
		ABaseWeapon* BW = Cast<ABaseWeapon>(WeaponInv[1]);
		if (BW)
		{
			Character->CombatComp->Equip(BW);
			InvIndex = 1;
		}
	}
	else
	{
		ServerEquipWeaponTwo();
	}
}

void UInventoryComponent::ServerEquipWeaponThree_Implementation()
{
	EquipWeaponThree();
}

void UInventoryComponent::EquipWeaponThree()
{
	if (Character->HasAuthority())
	{
		ABaseWeapon* BW = Cast<ABaseWeapon>(WeaponInv[2]);
		if (BW)
		{
			Character->CombatComp->Equip(BW);
			InvIndex = 2;
		}
	}
	else
	{
		ServerEquipWeaponThree();
	}
}

void UInventoryComponent::EquipPreviousWeapon()
{
	if (GetCurrentWeaponIndex() > 0)
	{
		for (int32 i = GetCurrentWeaponIndex(); i > 0; i--)
		{
			ABaseWeapon* BW = Cast<ABaseWeapon>(WeaponInv[i - 1]);
			if (BW)
			{
				InvIndex = i - 1;
				Character->CombatComp->Equip(BW);
				return;
			}
		}
	}
}

void UInventoryComponent::EquipNextWeapon()
{
	if (GetCurrentWeaponIndex() < 2)
	{
		for (int32 i = GetCurrentWeaponIndex(); i < WeaponInv.Num() - 1; i++)
		{
			ABaseWeapon* BW = Cast<ABaseWeapon>(WeaponInv[i + 1]);
			if (BW)
			{
				InvIndex = i + 1;
				Character->CombatComp->Equip(BW);
				return;
			}
		}
	}
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UInventoryComponent::DisplayInventory()
{

}

void UInventoryComponent::StopShowingInventory()
{

}

void UInventoryComponent::SwapIndex(int32 IndexOne, int32 IndexToSwap)
{
	WeaponInv.Swap(IndexOne, IndexToSwap);
}

int32 UInventoryComponent::IndexOfWeaponAdded(int32 Index)
{
	return Index;
}

void UInventoryComponent::InInventoryHandle(ABaseWeapon* Weapon)
{
	if (Character->HasAuthority())
	{
		if (Weapon)
		{
			Weapon->Mesh->SetVisibility(false);
			Character->CombatComp->AttachToPawn(Weapon);
			Weapon->SetWeaponState(EWeaponState::EWS_InInventory);
		}
	}
}