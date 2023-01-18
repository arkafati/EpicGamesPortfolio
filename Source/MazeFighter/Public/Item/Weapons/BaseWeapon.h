// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Item.h"
#include "BaseWeapon.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial			UMETA(DisplayName = "Initial"),
	EWS_InInventory		UMETA(DisplayName = "InInventory"),
	EWS_Equipped		UMETA(DisplayName = "Equipped"),
	EWS_Dropped			UMETA(DisplayName = "Dropped"),

	EWS_MAX				UMETA(DisplayName = "DefaultMAX")
};
UENUM(BlueprintType)
enum class EPrimaryAttackType : uint8
{
	EPAT_None			UMETA(DisplayName = "Initial"),
	EPAT_HitScanFire	UMETA(DisplayName = "HitScanFire"),
	EPAT_ShotgunFire	UMETA(DisplayName = "ShotgunFire"),

	EPAT_MAX			UMETA(DisplayName = "DefaultMAX")
};
UENUM(BlueprintType)
enum class ESecondaryAttackType : uint8
{
	ESAT_None			UMETA(DisplayName = "Initial"),
	ESAT_Aim			UMETA(DisplayName = "Aim"),
	ESAT_DualFire		UMETA(DisplayName = "DualFire"),

	ESAT_MAX			UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class MAZEFIGHTER_API ABaseWeapon : public AItem
{
	GENERATED_BODY()

public:
	ABaseWeapon(const FObjectInitializer& ObjectInitializer);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon | Damage")
	float BaseDamage;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon | Functions")
	bool bCanPrimaryAttack;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon | Functions")
	bool bCanSecondaryAttack;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Functions")
	bool bAutomatic;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Functions")
	bool bBurst;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Functions")
	float FireRateTimer;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Functions")
	float BurstRateTimer;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Functions")
	float BulletsPerBurst;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon | Ammo")
	float WeaponAmmo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon | Aim")
	bool bFirstPerson;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon | Aim")
	float ADSSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon | Recoil")
	float RecoilLowerBound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon | Recoil")
	float RecoilUpperBound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon | Movement")
	float MovementSpeedMultiplier;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Mesh")
	class USkeletalMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UGameplayEffect> DamageGameplayEffect;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, EditAnywhere, Category = "Weapon|State")
	EWeaponState WeaponState;
	UPROPERTY(Replicated, EditAnywhere, Category = "Weapon|State")
	EPrimaryAttackType PrimaryAttack;
	UPROPERTY(Replicated, EditAnywhere, Category = "Weapon|State")
	ESecondaryAttackType SecondaryAttack;
	
	void HitScanFire(const FVector& HitTarget);
	void SetWeaponState(EWeaponState State);
	void OnWeaponState();

	UFUNCTION(BlueprintCallable)
	EWeaponState GetWeaponState();
	UFUNCTION(BlueprintCallable)
	EPrimaryAttackType GetPrimaryAttackType();
	UFUNCTION(BlueprintCallable)
	ESecondaryAttackType GetSecondaryAttackType();

	UFUNCTION()
	void OnRep_WeaponState();

	//  Apply Damage to Actor from a result of a line trace
	void ApplyDamage(FHitResult& HitResult);
};
