// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/GameplayAbilityCore.h"
#include "AbilitySystem/CharacterASC.h"

UGameplayAbilityCore::UGameplayAbilityCore()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Dead")));
}

void UGameplayAbilityCore::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (ActivateAbilityOnGranted)
	{
		ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
	}
}
