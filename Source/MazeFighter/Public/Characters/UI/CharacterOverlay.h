// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"

/**
 * 
 */
UCLASS()
class MAZEFIGHTER_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* ShieldBar;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* AttackText;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DefenseText;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SpeedText;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* RegenText;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* APText;
};
