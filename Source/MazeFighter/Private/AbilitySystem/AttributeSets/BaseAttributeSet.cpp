// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AttributeSets/BaseAttributeSet.h"
#include "Characters/Player/PlayerCharacter.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "Characters/BasePlayerController.h"
#include "Characters/Player/PlayerCharacter.h"
#include "Characters/BasePlayerController.h"


void UBaseAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, Shield, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, MaxShield, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, AP, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, MaxAP, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, Attack, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, MaxAttack, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, Defense, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, MaxDefense, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, Speed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, MaxSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, Regen, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, MaxRegen, COND_None, REPNOTIFY_Always);

}

void UBaseAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();
	const FGameplayTagContainer& SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
	FGameplayTagContainer SpecAssetTags;
	Data.EffectSpec.GetAllAssetTags(SpecAssetTags);

	// Get the Target actor, which should be our owner
	AActor* TargetActor = nullptr;
	AController* TargetController = nullptr;
	APlayerCharacter* TargetCharacter = nullptr;
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		TargetCharacter = Cast<APlayerCharacter>(TargetActor);
	}

	// Get the Source actor
	AActor* SourceActor = nullptr;
	AController* SourceController = nullptr;
	APlayerCharacter* SourceCharacter = nullptr;
	if (Source && Source->AbilityActorInfo.IsValid() && Source->AbilityActorInfo->AvatarActor.IsValid())
	{
		SourceActor = Source->AbilityActorInfo->AvatarActor.Get();
		SourceController = Source->AbilityActorInfo->PlayerController.Get();
		if (SourceController == nullptr && SourceActor != nullptr)
		{
			if (APawn* Pawn = Cast<APawn>(SourceActor))
			{
				SourceController = Pawn->GetController();
			}
		}

		// Use the controller to find the source pawn
		if (SourceController)
		{
			SourceCharacter = Cast<APlayerCharacter>(SourceController->GetPawn());
		}
		else
		{
			SourceCharacter = Cast<APlayerCharacter>(SourceActor);
		}

		// Set the causer actor based on context if it's set
		if (Context.GetEffectCauser())
		{
			SourceActor = Context.GetEffectCauser();
		}
	}

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// Try to extract a hit result
		FHitResult HitResult;
		if (Context.GetHitResult())
		{
			HitResult = *Context.GetHitResult();
		}

		// Store a local copy of the amount of damage done and clear the damage attribute
		float LocalDamageDone = GetDamage();
		SetDamage(0.f);

		if (LocalDamageDone > 0.0f)
		{
			// If character was alive before damage is added, handle damage
			// This prevents damage being added to dead things and replaying death animations
			bool WasAlive = true;

			if (TargetCharacter)
			{
				WasAlive = TargetCharacter->IsAlive();
			}

			if (!TargetCharacter->IsAlive())
			{

			}

			// Apply the health change and then clamp it
			const float NewShield = GetShield() - LocalDamageDone;
			float LeftoverDamage = NewShield * -1.f;
			if (NewShield >= 0.f)
			{
				LeftoverDamage = 0.f;
			}
			LeftoverDamage = FMath::Clamp(LeftoverDamage, 0.f, 10000.f);
			SetShield(FMath::Clamp(NewShield, 0.f, GetMaxShield()));


			const float NewHealth = GetHealth() - LeftoverDamage;
			SetHealth(FMath::Clamp(NewHealth, 0.0f, GetMaxHealth()));

			if (TargetCharacter && WasAlive)
			{
				// Show damage number for the Source player unless it was self damage
				if (SourceActor != TargetActor)
				{
					ABasePlayerController* PC = Cast<ABasePlayerController>(SourceController);
					if (PC)
					{

					}
				}
			}
		}
	}// Damage
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// Handle other health changes.
		// Health loss should go through Damage.
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
	} // Health
}

void UBaseAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, Health, OldHealth);

	ABasePlayerController* PC = Cast<ABasePlayerController>(GetOwningAbilitySystemComponent()->AbilityActorInfo->PlayerController);
	if (PC)
	{
		PC->SetHUDHealth(Health.GetBaseValue(), MaxHealth.GetBaseValue());
	}
}

void UBaseAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, MaxHealth, OldMaxHealth);
	ABasePlayerController* PC = Cast<ABasePlayerController>(GetOwningAbilitySystemComponent()->AbilityActorInfo->PlayerController);
	if (PC)
	{
		PC->SetHUDHealth(Health.GetBaseValue(), MaxHealth.GetBaseValue());
	}
}

void UBaseAttributeSet::OnRep_Shield(const FGameplayAttributeData& OldShield)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, Shield, OldShield);
	ABasePlayerController* PC = Cast<ABasePlayerController>(GetOwningAbilitySystemComponent()->AbilityActorInfo->PlayerController);
	if (PC)
	{
		PC->SetHUDShield(Shield.GetBaseValue(), MaxShield.GetBaseValue());
	}
}

void UBaseAttributeSet::OnRep_MaxShield(const FGameplayAttributeData& OldMaxShield)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, MaxShield, OldMaxShield);
	ABasePlayerController* PC = Cast<ABasePlayerController>(GetOwningAbilitySystemComponent()->AbilityActorInfo->PlayerController);
	if (PC)
	{
		PC->SetHUDShield(Shield.GetBaseValue(), MaxShield.GetBaseValue());
	}
}

void UBaseAttributeSet::OnRep_AP(const FGameplayAttributeData& OldAP)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, AP, OldAP);
	ABasePlayerController* PC = Cast<ABasePlayerController>(GetOwningAbilitySystemComponent()->AbilityActorInfo->PlayerController);
	if (PC)
	{
		PC->SetHUDAP(AP.GetBaseValue());
	}
}

void UBaseAttributeSet::OnRep_MaxAP(const FGameplayAttributeData& OldMaxAP)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, MaxAP, OldMaxAP);

}

void UBaseAttributeSet::OnRep_Attack(const FGameplayAttributeData& OldAttack)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, Attack, OldAttack);
	ABasePlayerController* PC = Cast<ABasePlayerController>(GetOwningAbilitySystemComponent()->AbilityActorInfo->PlayerController);
	if (PC)
	{
		PC->SetHUDAttack(Attack.GetBaseValue());
	}
}

void UBaseAttributeSet::OnRep_MaxAttack(const FGameplayAttributeData& OldMaxAttack)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, MaxAttack, OldMaxAttack);

}

void UBaseAttributeSet::OnRep_Defense(const FGameplayAttributeData& OldDefense)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, Defense, OldDefense);
	ABasePlayerController* PC = Cast<ABasePlayerController>(GetOwningAbilitySystemComponent()->AbilityActorInfo->PlayerController);
	if (PC)
	{
		PC->SetHUDDefense(Defense.GetBaseValue());
	}
}

void UBaseAttributeSet::OnRep_MaxDefense(const FGameplayAttributeData& OldMaxDefense)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, MaxDefense, OldMaxDefense);

}

void UBaseAttributeSet::OnRep_Speed(const FGameplayAttributeData& OldSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, Speed, OldSpeed);
	
	APlayerCharacter* Character = Cast<APlayerCharacter>(GetOwningAbilitySystemComponent()->AvatarActor);
	if (Character)
	{
		Character->CalculateSpeed(Speed.GetBaseValue());
	}
	ABasePlayerController* PC = Cast<ABasePlayerController>(GetOwningAbilitySystemComponent()->AbilityActorInfo->PlayerController);
	if (PC)
	{
		PC->SetHUDSpeed(Speed.GetBaseValue());
	}
}

void UBaseAttributeSet::OnRep_MaxSpeed(const FGameplayAttributeData& OldMaxSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, MaxSpeed, OldMaxSpeed);

}

void UBaseAttributeSet::OnRep_Regen(const FGameplayAttributeData& OldRegen)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, Regen, OldRegen);

	ABasePlayerController* PC = Cast<ABasePlayerController>(GetOwningAbilitySystemComponent()->AbilityActorInfo->PlayerController);
	if (PC)
	{
		PC->SetHUDRegen(Regen.GetBaseValue());
	}
}

void UBaseAttributeSet::OnRep_MaxRegen(const FGameplayAttributeData& OldMaxRegen)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, MaxRegen, OldMaxRegen);

}
