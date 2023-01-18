// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/CharacterASC.h"

void UCharacterASC::ReceiveDamage(UCharacterASC* SourceASC, float UnmitigatedDamage, float MitigatedDamage)
{
	ReceivedDamage.Broadcast(SourceASC, UnmitigatedDamage, MitigatedDamage);
}
