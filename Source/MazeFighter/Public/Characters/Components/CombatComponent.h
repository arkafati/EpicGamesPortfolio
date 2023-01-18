// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MAZEFIGHTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatComponent();
	friend class APlayerCharacter;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//Variables Used For Combat
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat | Bool")
	bool bPrimaryPressed = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat | Bool")
	bool bSecondaryPressed = false;
	UPROPERTY(ReplicatedUsing = OnRep_Aiming, VisibleAnywhere, BlueprintReadOnly, Category = "Combat | Weapon | Aim")
	bool bIsAiming = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat | Camera")
	bool bShouldSwitchCamera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon | Functions")
	float BulletsForBurst;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	class ABaseWeapon* EquippedWeapon;


	//OnRep Functions -- Used To Replicate Variables From Server To Client
	UFUNCTION()
	void OnRep_Aiming();


	//Combat Functions -- Called From Player Character Through Enhanced Input
	void PrimaryAttackPressed(bool bPressed);
	void SecondaryAttackPressed(bool bPressed);

	//Function Used To Determine The Correct Primary and Secondary Attack and Perform it
	void PrimaryAttack();
	void SecondaryAttack();

	//Function To Stop, Cancel, or Complete Primary or Secondary Attack Functions
	void StopSecondaryAttack();
	void StopPrimaryAttack();


	//Handle Hit Scan Weapon Fire
	void HitScanFire();
	UFUNCTION(Server, Reliable)
	void ServerHitScanFire(const FVector_NetQuantize& TraceHitTarget);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastHitScanFire(const FVector_NetQuantize& TraceHitTarget);

	void AddRandomRecoil();
	void StartFireTimer();
	void EndFireTimer();
	void StartBurstTimer();
	void EndBurstTimer();

	//Handle Aim Mechanics
	void Aim();
	UFUNCTION(Server, Reliable)
	void ServerAim();

	// Handle Stoppage of Aim Mechanics
	void StopAim();
	UFUNCTION(Server, Reliable)
	void ServerStopAim();

	void Reload(float AmmoInInventory);
	UFUNCTION(Server, Reliable)
	void ServerReload(float AmmoInInventory);

	//Used to Apply Damage through a Gameplay Effect that will decrease shield and attributes through attribute set
	void ApplyDamageEffect();
	
	void InterpBetweenCameras(float DeltaTime);

	virtual void AttachToPawn(class ABaseWeapon* Weapon);
	virtual void DropWeapon(class ABaseWeapon* Weapon);
	virtual void Equip(class ABaseWeapon* Weapon);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	class APlayerCharacter* Character;

	FVector HitTarget;
	FVector OriginalCameraPosition;
	FTimerHandle FireTimer;
	FTimerHandle BurstTimer;

};
