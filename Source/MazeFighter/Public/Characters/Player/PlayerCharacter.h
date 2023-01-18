// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "InputActionValue.h"
#include "PlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class MAZEFIGHTER_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter(const class FObjectInitializer& ObjectInitializer);

	virtual void PossessedBy(AController* NewController) override;

	class USpringArmComponent* GetCameraBoom();
	class UCameraComponent* GetFollowCamera();
	class UCameraComponent* GetFPSCamera();

	UFUNCTION(BlueprintCallable)
	float GetStartingCameraBoomArmLength();
	UFUNCTION(BlueprintCallable)
	FVector GetStartingCameraBoomLocation();

	void CalculateSpeed(float Value);
	UFUNCTION(Server, Reliable)
	void ServerCalculateSpeed();

protected:
	virtual void PostInitializeComponents() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintreadOnly, Category = "Camera")
	class UCameraComponent* FollowCamera;
	UPROPERTY(EditAnywhere, BlueprintreadWrite, Category = "Camera")
	class UCameraComponent* FPSCamera;

	virtual void OnRep_PlayerState() override;

	bool ASCInputBound;
	void BindASCInput();

	virtual void PawnClientRestart() override;
	virtual void BeginPlay() override;
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = true))
	class UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = true))
	class UInputAction* CrouchAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = true))
	class UInputAction* SprintAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = true))
	class UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = true))
	class UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = true))
	class UInputAction* PickupAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = true))
	class UInputAction* WeaponOneAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = true))
	class UInputAction* WeaponTwoAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = true))
	class UInputAction* WeaponThreeAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = true))
	class UInputAction* PrimaryAttackAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = true))
	class UInputAction* SecondaryAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = true))
	class UInputMappingContext* BaseMappingContext;

	void EnhancedCrouch();
	void EnhancedLook(const FInputActionValue& Value);
	void EnhancedMove(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable)
	void ServerEnhancedSprint();
	void EnhancedSprint();

	void EnhancedPickup();
	void EnhancedWeaponOne();
	void EnhancedWeaponTwo();
	void EnhancedWeaponThree();

	void EnhancedPrimaryAttack();
	void EnhancedPrimaryAttackStop();

	void EnhancedSecondaryAttack();
	void EnhancedSecondaryAttackStop();

	bool bIsCrouching;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = true))
	bool bIsSprinting;
};
