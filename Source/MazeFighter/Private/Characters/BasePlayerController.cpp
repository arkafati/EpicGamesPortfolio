// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BasePlayerController.h"
#include "Characters/BasePlayerState.h"
#include "AbilitySystemComponent.h"
#include "Characters/CharacterHUD.h"
#include "Characters/UI/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void ABasePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ABasePlayerState* PS = GetPlayerState<ABasePlayerState>();
	if (PS)
	{
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, InPawn);
	}
}

void ABasePlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	CharacterHUD = CharacterHUD == nullptr ? Cast<ACharacterHUD>(GetHUD()) : CharacterHUD;
	if (CharacterHUD && CharacterHUD->CharacterOverlay && CharacterHUD->CharacterOverlay->HealthBar)
	{
		const float HealthPercent = Health / MaxHealth;
		CharacterHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
	}
}

void ABasePlayerController::SetHUDShield(float Shield, float MaxShield)
{
	CharacterHUD = CharacterHUD == nullptr ? Cast<ACharacterHUD>(GetHUD()) : CharacterHUD;
	if (CharacterHUD && CharacterHUD->CharacterOverlay && CharacterHUD->CharacterOverlay->ShieldBar)
	{
		const float ShieldPercent = Shield / MaxShield;
		CharacterHUD->CharacterOverlay->ShieldBar->SetPercent(ShieldPercent);
	}
}

void ABasePlayerController::SetHUDSpeed(float Speed)
{
	CharacterHUD = CharacterHUD == nullptr ? Cast<ACharacterHUD>(GetHUD()) : CharacterHUD;
	if (CharacterHUD && CharacterHUD->CharacterOverlay && CharacterHUD->CharacterOverlay->SpeedText)
	{
		FString SpeedValue = FString::Printf(TEXT("%d"), FMath::CeilToInt(Speed));
		CharacterHUD->CharacterOverlay->SpeedText->SetText(FText::FromString(SpeedValue));
	}
}

void ABasePlayerController::SetHUDDefense(float Defense)
{
	CharacterHUD = CharacterHUD == nullptr ? Cast<ACharacterHUD>(GetHUD()) : CharacterHUD;
	if (CharacterHUD && CharacterHUD->CharacterOverlay && CharacterHUD->CharacterOverlay->DefenseText)
	{
		FString DefenseValue = FString::Printf(TEXT("%d"), FMath::CeilToInt(Defense));
		CharacterHUD->CharacterOverlay->DefenseText->SetText(FText::FromString(DefenseValue));
	}
}

void ABasePlayerController::SetHUDAttack(float Attack)
{
	CharacterHUD = CharacterHUD == nullptr ? Cast<ACharacterHUD>(GetHUD()) : CharacterHUD;
	if (CharacterHUD && CharacterHUD->CharacterOverlay && CharacterHUD->CharacterOverlay->AttackText)
	{
		FString AttackValue = FString::Printf(TEXT("%d"), FMath::CeilToInt(Attack));
		CharacterHUD->CharacterOverlay->AttackText->SetText(FText::FromString(AttackValue));
	}
}

void ABasePlayerController::SetHUDRegen(float Regen)
{
	CharacterHUD = CharacterHUD == nullptr ? Cast<ACharacterHUD>(GetHUD()) : CharacterHUD;
	if (CharacterHUD && CharacterHUD->CharacterOverlay && CharacterHUD->CharacterOverlay->RegenText)
	{
		FString RegenValue = FString::Printf(TEXT("%d"), FMath::CeilToInt(Regen));
		CharacterHUD->CharacterOverlay->RegenText->SetText(FText::FromString(RegenValue));
	}
}

void ABasePlayerController::SetHUDAP(float AP)
{
	CharacterHUD = CharacterHUD == nullptr ? Cast<ACharacterHUD>(GetHUD()) : CharacterHUD;
	if (CharacterHUD && CharacterHUD->CharacterOverlay && CharacterHUD->CharacterOverlay->APText)
	{
		FString APValue = FString::Printf(TEXT("%d"), FMath::CeilToInt(AP));
		CharacterHUD->CharacterOverlay->APText->SetText(FText::FromString(APValue));
	}
}

void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();

	CharacterHUD = Cast<ACharacterHUD>(GetHUD());


}
