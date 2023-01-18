// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BasePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MAZEFIGHTER_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void OnPossess(APawn* InPawn) override;
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDShield(float Shield, float MaxShield);
	void SetHUDSpeed(float Speed);
	void SetHUDDefense(float Defense);
	void SetHUDAttack(float Attack);
	void SetHUDRegen(float Regen);
	void SetHUDAP(float AP);

protected:
	virtual void BeginPlay() override;

private:
	class ACharacterHUD* CharacterHUD;
};
