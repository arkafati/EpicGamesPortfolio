// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "AbilitySystem/CharacterASC.h"
#include "AbilitySystem/GameplayAbilityCore.h"
#include "AbilitySystem/AttributeSets/BaseAttributeSet.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Item/Item.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
#include "Item/Weapons/RangedWeapon.h"
#include "Item/Weapons/MeleeWeapon.h"
#include "Characters/Components/InventoryComponent.h"
#include "Characters/Components/CombatComponent.h"
#include "Item/Weapons/BaseWeapon.h"
#include "Characters/BasePlayerController.h"

//Constructor
ABaseCharacter::ABaseCharacter(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<UCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Overlap);

	bAlwaysRelevant = true;

	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
	EffectRemoveOnDeathTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));

	InventoryComp = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComp"));
	InventoryComp->SetIsReplicated(true);

	CombatComp = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComp"));
	CombatComp->SetIsReplicated(true);

}

//Called at the beginning of the game
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FHitResult HitResult;
	ItemTrace(HitResult);
}

//Replicated Objects
void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABaseCharacter, ItemsToPickup, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ABaseCharacter, ItemHitLastFrame, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ABaseCharacter, bShouldTraceItem, COND_OwnerOnly);


}

//Ability System Component Getter
UAbilitySystemComponent* ABaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

//Line Traces
void ABaseCharacter::TraceUnderCrosshair(FHitResult& OutHitResult)
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation, CrosshairWorldPosition, CrosshairWorldDirection);

	if (bScreenToWorld)
	{
		FVector Start{ CrosshairWorldPosition };
		float DistanceToCharacter = (GetActorLocation() - Start).Size();
		Start += CrosshairWorldDirection * (DistanceToCharacter + 100.f);

		FVector End{ Start + CrosshairWorldDirection * 80000.f };

		GetWorld()->LineTraceSingleByChannel(
			OutHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility);

		DrawDebugSphere(GetWorld(), OutHitResult.ImpactPoint, 12.f, 6.f, FColor::Cyan);
	}


}

void ABaseCharacter::ItemTrace(FHitResult& OutHitResult)
{
	if (bShouldTraceItem)
	{
		TraceUnderCrosshair(OutHitResult);
		if (OutHitResult.bBlockingHit)
		{
			AItem* HitItem = Cast<AItem>(OutHitResult.GetActor());
			if (ItemsToPickup.Contains(HitItem) && HitItem != nullptr)
			{
				if (HitItem)
				{
					HitItem->GetPickupWidget()->SetVisibility(true);
				}
				if (ItemHitLastFrame)
				{
					if (HitItem != ItemHitLastFrame)
					{
						ItemHitLastFrame->GetPickupWidget()->SetVisibility(false);
					}
				}
				ItemHitLastFrame = HitItem;
			}
			else
			{
				if (ItemHitLastFrame && ItemHitLastFrame->PickupWidget)
				{
					ItemHitLastFrame->GetPickupWidget()->SetVisibility(false);
					ItemHitLastFrame = nullptr;
				}
			}
		}
	}
	else if (ItemHitLastFrame)
	{
		ItemHitLastFrame->GetPickupWidget()->SetVisibility(false);
		ItemHitLastFrame = nullptr;
	}
}

//Pickup Item Information Functions
void ABaseCharacter::OnRep_ShouldTrace()
{
	if (bShouldTraceItem)
	{
		if (ItemsToPickup.IsEmpty())
		{
			bShouldTraceItem = false;
		}
		else
		{
			bShouldTraceItem = true;
		}
	}
}

void ABaseCharacter::OnRep_LastItemHit(AItem* Item)
{
	if (Item)
	{
		ItemHitLastFrame = Item;
	}
	else
	{
		ItemHitLastFrame = nullptr;
	}

}

void ABaseCharacter::AddItemToPickup(AItem* Item)
{
	if (Item)
	{
		ItemsToPickup.Add(Item);
		if (ItemsToPickup.IsEmpty())
		{
			bShouldTraceItem = false;
		}
		else
		{
			bShouldTraceItem = true;
		}
	}
}

void ABaseCharacter::SubtractItemToPickup(AItem* Item)
{
	if (Item)
	{
		if (ItemsToPickup.Contains(Item))
		{
			ItemsToPickup.Remove(Item);
		}
		if (ItemsToPickup.IsEmpty())
		{
			bShouldTraceItem = false;
		}
		else
		{
			bShouldTraceItem = true;
		}
	}
}

//Equipped Weapon Information
bool ABaseCharacter::IsWeaponEquipped()
{
	if (CombatComp && CombatComp->EquippedWeapon)
	{
		return true;
	}
	return false;
}

ABaseWeapon* ABaseCharacter::GetEquippedWeapon()
{
	if (CombatComp && CombatComp->EquippedWeapon)
	{
		return CombatComp->EquippedWeapon;
	}

	return nullptr;
}

//Add and Remove Abilities and Effects
void ABaseCharacter::AddCharacterAbilities()
{
	if (!HasAuthority() || !AbilitySystemComponent.IsValid() || AbilitySystemComponent->CharacterAbilitiesGiven)
	{
		return;
	}

	for (TSubclassOf<UGameplayAbilityCore>& StartupAbility : CharacterAbilities)
	{
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(StartupAbility, GetAbilityLevel(StartupAbility.GetDefaultObject()->AbilityID), static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID), this));
	}

	AbilitySystemComponent->CharacterAbilitiesGiven = true;
}

void ABaseCharacter::AddStartupEffects()
{
	if (!AbilitySystemComponent.IsValid() || !HasAuthority() || AbilitySystemComponent->StartupEffectsApplied)
	{
		return;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	for (TSubclassOf<UGameplayEffect> GameplayEffect : StartupEffects)
	{
		FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributes, 1, EffectContext);
		if (NewHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent.Get());
		}
	}

	AbilitySystemComponent->StartupEffectsApplied = true;

}

void ABaseCharacter::RemoveCharacterAbilities()
{
	if (!HasAuthority() || !AbilitySystemComponent.IsValid() || !AbilitySystemComponent->CharacterAbilitiesGiven)
	{
		return;
	}

	TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;
	for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
	{
		if ((Spec.SourceObject == this) && CharacterAbilities.Contains(Spec.Ability->GetClass()))
		{
			AbilitiesToRemove.Add(Spec.Handle);
		}
	}

	for (int32 i = 0; i < AbilitiesToRemove.Num(); i++)
	{
		AbilitySystemComponent->ClearAbility(AbilitiesToRemove[i]);
	}

	AbilitySystemComponent->CharacterAbilitiesGiven = false;
}

//Initialize Attributes
void ABaseCharacter::InitializeAttributes()
{
	if (!AbilitySystemComponent.IsValid())
	{
		return;
	}
	if (!DefaultAttributes)
	{
		return;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributes, 1, EffectContext);
	if (NewHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent.Get());
	}

	AbilitySystemComponent->AttributesInitialized = true;
}

//Validates that we are not dead and death functions
bool ABaseCharacter::IsAlive() const
{
	return GetHealth() > 0.0f;
}

void ABaseCharacter::Die()
{
	RemoveCharacterAbilities();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	OnCharacterDied.Broadcast(this);

	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->CancelAbilities();

		FGameplayTagContainer EffectsTagsToRemove;
		EffectsTagsToRemove.AddTag(EffectRemoveOnDeathTag);
		int32 NumEffectsRemoved = AbilitySystemComponent->RemoveActiveEffectsWithTags(EffectsTagsToRemove);
		AbilitySystemComponent->AddLooseGameplayTag(DeadTag);
	}

	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}
	else
	{
		FinishDying();
	}
}

void ABaseCharacter::FinishDying()
{
	Destroy();
}

//Attribute Getters
int32 ABaseCharacter::GetAbilityLevel(MazeFAbilityID AbilityID) const
{
	return 1;
}

float ABaseCharacter::GetHealth() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetHealth();
	}

	return 0.f;
}

float ABaseCharacter::GetMaxHealth() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetMaxHealth();
	}

	return 0.f;
}

float ABaseCharacter::GetShield() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetShield();
	}

	return 0.f;
}

float ABaseCharacter::GetMaxShield() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetMaxShield();
	}

	return 0.f;
}

float ABaseCharacter::GetAP() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetAP();
	}

	return 0.f;
}

float ABaseCharacter::GetMaxAP() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetMaxAP();
	}

	return 0.f;
}

float ABaseCharacter::GetAttack() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetAttack();
	}

	return 0.f;
}

float ABaseCharacter::GetMaxAttack() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetMaxAttack();
	}

	return 0.f;
}

float ABaseCharacter::GetDefense() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetDefense();
	}

	return 0.f;
}

float ABaseCharacter::GetMaxDefense() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetMaxDefense();
	}

	return 0.f;
}

float ABaseCharacter::GetSpeed() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetSpeed();
	}

	return 0.f;
}

float ABaseCharacter::GetMaxSpeed() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetMaxSpeed();
	}

	return 0.f;
}

float ABaseCharacter::GetRegen() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetRegen();
	}

	return 0.f;
}

float ABaseCharacter::GetMaxRegen() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetMaxRegen();
	}

	return 0.f;
}

void ABaseCharacter::SetHealth(float Health)
{
	if (AttributeSetBase.IsValid())
	{
		AttributeSetBase->SetHealth(Health);
	}
}

void ABaseCharacter::SetShield(float Shield)
{
	if (AttributeSetBase.IsValid())
	{
		AttributeSetBase->SetShield(Shield);
	}
}

void ABaseCharacter::SetAP(float AP)
{
	if (AttributeSetBase.IsValid())
	{
		AttributeSetBase->SetAP(AP);
	}
}