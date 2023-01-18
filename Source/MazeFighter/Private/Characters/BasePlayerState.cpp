// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BasePlayerState.h"
#include "AbilitySystem/CharacterASC.h"
#include "AbilitySystem/AttributeSets/BaseAttributeSet.h"
#include "Characters/Player/PlayerCharacter.h"

ABasePlayerState::ABasePlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UCharacterASC>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSetBase = CreateDefaultSubobject<UBaseAttributeSet>(TEXT("AttributeSetBase"));

	NetUpdateFrequency = 100.f;

	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));

}

UAbilitySystemComponent* ABasePlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UBaseAttributeSet* ABasePlayerState::GetAttributeSetBase() const
{
	return AttributeSetBase;
}

bool ABasePlayerState::IsAlive() const
{
	return GetHealth() > 0.0f;
}

float ABasePlayerState::GetHealth() const
{
	return AttributeSetBase->GetHealth();
}

float ABasePlayerState::GetMaxHealth() const
{
	return AttributeSetBase->GetMaxHealth();
}

float ABasePlayerState::GetShield() const
{
	return AttributeSetBase->GetShield();
}

float ABasePlayerState::GetMaxShield() const
{
	return AttributeSetBase->GetMaxShield();
}

float ABasePlayerState::GetAP() const
{
	return AttributeSetBase->GetAP();
}

float ABasePlayerState::GetMaxAP() const
{
	return AttributeSetBase->GetMaxAP();
}

float ABasePlayerState::GetAttack() const
{
	return AttributeSetBase->GetAttack();
}

float ABasePlayerState::GetMaxAttack() const
{
	return AttributeSetBase->GetMaxAttack();
}

float ABasePlayerState::GetDefense() const
{
	return AttributeSetBase->GetDefense();
}

float ABasePlayerState::GetMaxDefense() const
{
	return AttributeSetBase->GetMaxDefense();
}

float ABasePlayerState::GetSpeed() const
{
	return AttributeSetBase->GetSpeed();
}

float ABasePlayerState::GetMaxSpeed() const
{
	return AttributeSetBase->GetMaxSpeed();
}

float ABasePlayerState::GetRegen() const
{
	return AttributeSetBase->GetRegen();
}

float ABasePlayerState::GetMaxRegen() const
{
	return AttributeSetBase->GetMaxRegen();
}

void ABasePlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &ABasePlayerState::HealthChanged);
	}
}

void ABasePlayerState::HealthChanged(const FOnAttributeChangeData& Data)
{
	float Health = Data.NewValue;

	// Update floating status bar
	APlayerCharacter* Player = Cast<APlayerCharacter>(GetPawn());

	// If the player died, handle death
	if (!IsAlive() && !AbilitySystemComponent->HasMatchingGameplayTag(DeadTag) && AbilitySystemComponent->AttributesInitialized)
	{
		if (Player)
		{
			Player->Die();
		}
	}
}
