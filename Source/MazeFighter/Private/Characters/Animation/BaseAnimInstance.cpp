// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Animation/BaseAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Characters/Player/PlayerCharacter.h"
#include "Item/Item.h"
#include "Item/Weapons/BaseWeapon.h"
#include "Characters/Components/CombatComponent.h"

void UBaseAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	Character = Cast<APlayerCharacter>(TryGetPawnOwner());
}
void UBaseAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);
	if (Character == nullptr)
	{
		Character = Cast<APlayerCharacter>(TryGetPawnOwner());
	}
	if (Character == nullptr) return;
	FVector Velocity = Character->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	bIsInAir = Character->GetCharacterMovement()->IsFalling();
	bIsAccelerating = Character->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;

	bIsCrouched = Character->bIsCrouched;

	// Offset Yaw for Strafing
	FRotator AimRotation = Character->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(Character->GetVelocity());
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 6.f);
	YawOffset = DeltaRotation.Yaw;

	bWeaponEquipped = Character->IsWeaponEquipped();
	EquippedWeapon = Character->CombatComp->EquippedWeapon;

}
