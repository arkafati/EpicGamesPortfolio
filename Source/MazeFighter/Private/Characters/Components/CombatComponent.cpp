// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Components/CombatComponent.h"
#include "Characters/Player/PlayerCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Camera/CameraComponent.h"
#include "Item/Weapons/BaseWeapon.h"
#include "Item/Item.h"
#include "Item/Weapons/RangedWeapon.h"
#include "Math/UnrealMathUtility.h"
#include "TimerManager.h"
#include "Engine/SkeletalMeshSocket.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(true);
	// ...
}

// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, bIsAiming);
	DOREPLIFETIME(UCombatComponent, EquippedWeapon);

}


// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Character && Character->IsLocallyControlled())
	{
		FHitResult HitResult;
		Character->TraceUnderCrosshair(HitResult);
		HitTarget = HitResult.ImpactPoint;


		InterpBetweenCameras(DeltaTime);

	}
	// ...
}

void UCombatComponent::Equip(ABaseWeapon* Weapon)
{
	if (!Character->HasAuthority()) return;

	if (Weapon && EquippedWeapon != Weapon)
	{
		if (EquippedWeapon && EquippedWeapon->WeaponState == EWeaponState::EWS_Dropped)
		{
			EquippedWeapon = nullptr;
		}

		//CheckToSeeIfWeaponIsEquipped
		if (EquippedWeapon != nullptr)
		{
			EquippedWeapon->SetWeaponState(EWeaponState::EWS_InInventory);
			EquippedWeapon->Mesh->SetVisibility(false);
		}
		EquippedWeapon = Weapon;
		EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
		EquippedWeapon->Mesh->SetVisibility(true);
	}
}

void UCombatComponent::DropWeapon(ABaseWeapon* Weapon)
{
	if (!Character->HasAuthority()) return;

	FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, true);
	EquippedWeapon->Mesh->DetachFromComponent(DetachmentTransformRules);
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Dropped);

	EquippedWeapon->Mesh->SetVisibility(true);
	EquippedWeapon->Mesh->SetSimulatePhysics(true);
	EquippedWeapon->Mesh->SetEnableGravity(true);

	const FRotator Rotation = Character->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	EquippedWeapon->Mesh->AddImpulse(Direction * 200.f, NAME_None, true);
	EquippedWeapon->Mesh->AddImpulse(FVector(0.f, 0.f, 250.f), NAME_None, true);
}

void UCombatComponent::AttachToPawn(ABaseWeapon* Weapon)
{
	if (!Character->HasAuthority()) return;

	if (Weapon)
	{
		const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("GunMeshSocket"));
		if (HandSocket)
		{
			Weapon->Mesh->SetSimulatePhysics(false);
			Weapon->Mesh->SetEnableGravity(false);
			HandSocket->AttachActor(Weapon, Character->GetMesh());
			Weapon->SetOwner(Character);
			Weapon->SetWeaponState(EWeaponState::EWS_InInventory);
		}
	}
}

void UCombatComponent::PrimaryAttackPressed(bool bPressed)
{
	bPrimaryPressed = bPressed;

	if (bPrimaryPressed)
	{
		PrimaryAttack();
	}
	else if (!bPrimaryPressed)
	{
		StopPrimaryAttack();
	}
}

void UCombatComponent::SecondaryAttackPressed(bool bPressed)
{
	bSecondaryPressed = bPressed;

	if (bSecondaryPressed)
	{
		SecondaryAttack();
	}
	else if(!bSecondaryPressed)
	{
		StopSecondaryAttack();
	}
}

void UCombatComponent::PrimaryAttack()
{
	if (EquippedWeapon)
	{
		switch (EquippedWeapon->GetPrimaryAttackType())
		{
		case EPrimaryAttackType::EPAT_HitScanFire:
			HitScanFire();
			break;
		}
	}
}

void UCombatComponent::SecondaryAttack()
{
	if (EquippedWeapon)
	{
		switch (EquippedWeapon->GetSecondaryAttackType())
		{
		case ESecondaryAttackType::ESAT_Aim:
			Aim();
			break;
		}
	}
}

void UCombatComponent::StopPrimaryAttack()
{

}

void UCombatComponent::StopSecondaryAttack()
{
	if (EquippedWeapon)
	{
		switch (EquippedWeapon->GetSecondaryAttackType())
		{
		case ESecondaryAttackType::ESAT_Aim:
			StopAim();
			break;
		}
	}
	else
	{

	}
}

void UCombatComponent::HitScanFire()
{
	if (EquippedWeapon->bCanPrimaryAttack)
	{
		EquippedWeapon->bCanPrimaryAttack = false;
		if (EquippedWeapon->WeaponAmmo > 0.f)
		{
			ServerHitScanFire(HitTarget);
			AddRandomRecoil();
			if (EquippedWeapon->bBurst)
			{
				StartBurstTimer();
			}
			else
			{
				StartFireTimer();
			}
			EquippedWeapon->WeaponAmmo -= 1;
		}	
	}

}

void UCombatComponent::ServerHitScanFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	MulticastHitScanFire(TraceHitTarget);
}

void UCombatComponent::MulticastHitScanFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if (EquippedWeapon)
	{
		EquippedWeapon->HitScanFire(TraceHitTarget);
	}
}

void UCombatComponent::AddRandomRecoil()
{
	Character->AddControllerYawInput(FMath::FRandRange(-.2f, .4f));
	Character->AddControllerPitchInput(FMath::FRandRange(-.4f, -.1f));
}

void UCombatComponent::StartFireTimer()
{
	if (Character && EquippedWeapon)
	{
		Character->GetWorldTimerManager().SetTimer(FireTimer, this, &UCombatComponent::EndFireTimer, EquippedWeapon->FireRateTimer);

	}
}

void UCombatComponent::EndFireTimer()
{
	if (EquippedWeapon == nullptr) return;
	if (bPrimaryPressed && EquippedWeapon->bAutomatic)
	{
		EquippedWeapon->bCanPrimaryAttack = true;
		PrimaryAttack();
	}

	EquippedWeapon->bCanPrimaryAttack = true;
}

void UCombatComponent::StartBurstTimer()
{
	if (Character && EquippedWeapon)
	{
		Character->GetWorldTimerManager().SetTimer(BurstTimer, this, &UCombatComponent::EndBurstTimer, EquippedWeapon->BurstRateTimer);
	}
}

void UCombatComponent::EndBurstTimer()
{
	if (EquippedWeapon == nullptr) return;
	if (EquippedWeapon->bBurst)
	{
		BulletsForBurst += 1.f;
		if (BulletsForBurst < EquippedWeapon->BulletsPerBurst)
		{
			EquippedWeapon->bCanPrimaryAttack = true;
			PrimaryAttack();
		}
		else if (BulletsForBurst == EquippedWeapon->BulletsPerBurst)
		{
			StartFireTimer();
			BulletsForBurst = 0.f;
		}
	}
}

void UCombatComponent::Aim()
{
	if (Character->HasAuthority())
	{
		bIsAiming = true;

		if (Character->IsLocallyControlled())
		{
			if (EquippedWeapon->bFirstPerson && !Character->GetFPSCamera()->IsActive())
			{
				bShouldSwitchCamera = true;
			}
			if (bIsAiming && !EquippedWeapon->bFirstPerson)
			{
				Character->GetFollowCamera()->SetFieldOfView(50.f);
			}
			else if(!bIsAiming)
			{
				Character->GetFollowCamera()->SetFieldOfView(80.f);
			}
		}
	}
	else
	{
		ServerAim();
	}

}

void UCombatComponent::ServerAim_Implementation()
{
	Aim();
}

void UCombatComponent::StopAim()
{
	if (Character->HasAuthority())
	{
		bIsAiming = false;

		if (Character->IsLocallyControlled())
		{
			if (Character->GetFPSCamera()->IsActive())
			{
				bShouldSwitchCamera = true;
			}
			if (bIsAiming && !EquippedWeapon->bFirstPerson)
			{
				Character->GetFollowCamera()->SetFieldOfView(50.f);
			}
			else
			{
				Character->GetFollowCamera()->SetFieldOfView(80.f);
			}
		}
	}
	else
	{
		ServerStopAim();
	}
}

void UCombatComponent::ServerStopAim_Implementation()
{
	StopAim();
}

void UCombatComponent::OnRep_Aiming()
{
	if (bIsAiming)
	{
		bIsAiming = true;

		if (EquippedWeapon->bFirstPerson && !Character->GetFPSCamera()->IsActive())
		{
			bShouldSwitchCamera = true;
		}
		else
		{
			Character->GetFollowCamera()->SetFieldOfView(50.f);
		}
	}
	else
	{
		bIsAiming = false;	

		if (Character->GetFPSCamera()->IsActive())
		{
			bShouldSwitchCamera = true;
		}
		else
		{
			Character->GetFollowCamera()->SetFieldOfView(80.f);
		}

	}
}

void UCombatComponent::Reload(float AmmoInInventory)
{
}

void UCombatComponent::ServerReload_Implementation(float AmmoInInventory)
{
}

void UCombatComponent::ApplyDamageEffect()
{

}

void UCombatComponent::InterpBetweenCameras(float DeltaTime)
{
	if (bShouldSwitchCamera)
	{
		if (Character->GetFollowCamera()->IsActive() && bIsAiming)
		{
			FVector UpdateLocation = FMath::VInterpTo(Character->GetFollowCamera()->GetComponentLocation(), Character->GetFPSCamera()->GetComponentLocation(), DeltaTime, 25.f);
			Character->GetFollowCamera()->SetWorldLocation(UpdateLocation);
			if (Character->GetFollowCamera()->GetComponentLocation().Equals(Character->GetFPSCamera()->GetComponentLocation(), 1.f))
			{
				Character->GetFPSCamera()->SetActive(true);
				Character->GetFollowCamera()->SetActive(false);
				bShouldSwitchCamera = false;
			}

		}
		else if (Character->GetFPSCamera()->IsActive() || !Character->GetFollowCamera()->GetRelativeLocation().Equals(FVector(0.f, 0.f, 0.f)))
		{
			Character->GetFollowCamera()->SetActive(true);
			Character->GetFPSCamera()->SetActive(false);

			FVector OriginalRelativeLocation = FMath::VInterpTo(Character->GetFollowCamera()->GetRelativeLocation(), FVector(0.f, 0.f, 0.f), DeltaTime, 25.f);
			Character->GetFollowCamera()->SetRelativeLocation(OriginalRelativeLocation);
			if (Character->GetFollowCamera()->GetRelativeLocation().Equals(FVector(0.f, 0.f, 0.f)))
			{
				bShouldSwitchCamera = false;
			}
		}
	}
}

