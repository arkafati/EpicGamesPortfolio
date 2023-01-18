// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Player/PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Characters/BasePlayerState.h"
#include "Characters/BasePlayerController.h"
#include "Components/CapsuleComponent.h"
#include "AbilitySystem/CharacterASC.h"
#include "AbilitySystem/AttributeSets/BaseAttributeSet.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystemInterface.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputActionValue.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Characters/Components/InventoryComponent.h"
#include "Characters/Components/CombatComponent.h"

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bUsePawnControlRotation = true;
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);
	FollowCamera->FieldOfView = 80.f;

	FPSCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FPSCamera"));
	FPSCamera->SetupAttachment(GetMesh(), FName("head"));

	FollowCamera->SetActive(true);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionProfileName(FName("NoCollision"));

	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));

	ASCInputBound = false;
	bIsCrouching = false;
	bIsSprinting = false;

	GetCharacterMovement()->MaxWalkSpeed = 2000.f;

}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	ABasePlayerState* PS = GetPlayerState<ABasePlayerState>();
	if (PS)
	{
		AbilitySystemComponent = Cast<UCharacterASC>(PS->GetAbilitySystemComponent());

		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);

		AttributeSetBase = PS->GetAttributeSetBase();

		AddStartupEffects();
		AddCharacterAbilities();

		InitializeAttributes();
		CalculateSpeed(GetSpeed());


		AbilitySystemComponent->SetTagMapCount(DeadTag, 0);

		SetHealth(GetMaxHealth());
		SetAP(GetMaxAP());
		SetShield(50.f);


	}
}

USpringArmComponent* APlayerCharacter::GetCameraBoom()
{
	return CameraBoom;
}

UCameraComponent* APlayerCharacter::GetFollowCamera()
{
	return FollowCamera;
}

UCameraComponent* APlayerCharacter::GetFPSCamera()
{
	return FPSCamera;
}

float APlayerCharacter::GetStartingCameraBoomArmLength()
{
	return 0.0f;
}

FVector APlayerCharacter::GetStartingCameraBoomLocation()
{
	return FVector();
}

void APlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (InventoryComp)
	{
		InventoryComp->Character = this;
	}
	if (CombatComp)
	{
		CombatComp->Character = this;
	}
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent)

	if (UEnhancedInputComponent* PlayerEIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (JumpAction)
		{
			PlayerEIC->BindAction(JumpAction, ETriggerEvent::Started, this, &APlayerCharacter::Jump);
			PlayerEIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopJumping);
		}
		if (MoveAction)
		{
			PlayerEIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::EnhancedMove);
		}
		if (CrouchAction)
		{
			PlayerEIC->BindAction(CrouchAction, ETriggerEvent::Started, this, &APlayerCharacter::EnhancedCrouch);
		}
		if (LookAction)
		{
			PlayerEIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::EnhancedLook);
		}
		if (SprintAction)
		{
			PlayerEIC->BindAction(SprintAction, ETriggerEvent::Started, this, &APlayerCharacter::EnhancedSprint);
		}
		if (PickupAction)
		{
			PlayerEIC->BindAction(PickupAction, ETriggerEvent::Started, this, &APlayerCharacter::EnhancedPickup);
		}
		if (WeaponOneAction)
		{
			PlayerEIC->BindAction(WeaponOneAction, ETriggerEvent::Started, this, &APlayerCharacter::EnhancedWeaponOne);
		}
		if (WeaponTwoAction)
		{
			PlayerEIC->BindAction(WeaponTwoAction, ETriggerEvent::Started, this, &APlayerCharacter::EnhancedWeaponTwo);
		}
		if (WeaponThreeAction)
		{
			PlayerEIC->BindAction(WeaponThreeAction, ETriggerEvent::Started, this, &APlayerCharacter::EnhancedWeaponThree);

		}
		if (PrimaryAttackAction)
		{
			PlayerEIC->BindAction(PrimaryAttackAction, ETriggerEvent::Started, this, &APlayerCharacter::EnhancedPrimaryAttack);
			PlayerEIC->BindAction(PrimaryAttackAction, ETriggerEvent::Completed, this, &APlayerCharacter::EnhancedPrimaryAttackStop);

		}
		if (SecondaryAttackAction)
		{
			PlayerEIC->BindAction(SecondaryAttackAction, ETriggerEvent::Started, this, &APlayerCharacter::EnhancedSecondaryAttack);
			PlayerEIC->BindAction(SecondaryAttackAction, ETriggerEvent::Completed, this, &APlayerCharacter::EnhancedSecondaryAttackStop);

		}
	}

	BindASCInput();
}

void APlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	ABasePlayerState* PS = GetPlayerState<ABasePlayerState>();
	if (PS)
	{
		AbilitySystemComponent = Cast<UCharacterASC>(PS->GetAbilitySystemComponent());

		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);

		AttributeSetBase = PS->GetAttributeSetBase();

		InitializeAttributes();
		CalculateSpeed(GetSpeed());
		BindASCInput();

		AbilitySystemComponent->SetTagMapCount(DeadTag, 0);

		SetHealth(GetMaxHealth());
		SetAP(GetMaxAP());
		SetShield(50.f);

		PlayerController = Cast<ABasePlayerController>(GetController());
		if (PlayerController)
		{
			PlayerController->SetHUDHealth(GetHealth(), GetMaxHealth());
			PlayerController->SetHUDShield(GetShield(), GetMaxShield());
			PlayerController->SetHUDAttack(GetAttack());
			PlayerController->SetHUDDefense(GetDefense());
			PlayerController->SetHUDSpeed(GetSpeed());
			PlayerController->SetHUDRegen(GetRegen());
			PlayerController->SetHUDAP(GetAP());
		}
	}
}

void APlayerCharacter::BindASCInput()
{
	if (!ASCInputBound && AbilitySystemComponent.IsValid() && IsValid(InputComponent))
	{
		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, FGameplayAbilityInputBinds(FString("Confirm"),
			FString("Cancel"), FString("MazeFAbilityID"), static_cast<int32>(MazeFAbilityID::Confirm), static_cast<int32>(MazeFAbilityID::Cancel)));

		ASCInputBound = true;
	}
}

void APlayerCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();

	if (ABasePlayerController* PC = Cast<ABasePlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();

			Subsystem->AddMappingContext(BaseMappingContext, 0);
		}
	}
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();


}

void APlayerCharacter::EnhancedPrimaryAttack()
{
	if (CombatComp)
	{
		CombatComp->PrimaryAttackPressed(true);
	}
}

void APlayerCharacter::EnhancedPrimaryAttackStop()
{
	if (CombatComp)
	{
		CombatComp->PrimaryAttackPressed(false);
	}
}

void APlayerCharacter::EnhancedSecondaryAttack()
{
	if (CombatComp)
	{
		CombatComp->SecondaryAttackPressed(true);
	}
}

void APlayerCharacter::EnhancedSecondaryAttackStop()
{
	if (CombatComp)
	{
		CombatComp->SecondaryAttackPressed(false);
	}
}

void APlayerCharacter::EnhancedCrouch()
{
	if (!bIsCrouching)
	{
		Crouch();
		bIsCrouching = true;
		return;
	}
	if (bIsCrouching)
	{
		UnCrouch();
		bIsCrouching = false;
		return;
	}
}

void APlayerCharacter::EnhancedLook(const FInputActionValue& Value)
{
	AddControllerPitchInput(Value[1] * -1);
	AddControllerYawInput(Value[0]);
}

void APlayerCharacter::EnhancedMove(const FInputActionValue& Value)
{
	if (Value.GetMagnitude() != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), Value[1]);
		AddMovementInput(GetActorRightVector(), Value[0]);
	}
}

void APlayerCharacter::EnhancedSprint()
{
	if (!bIsSprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed *= 1.25;
		bIsSprinting = true;
	}
	else if (bIsSprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed /= 1.25;
		bIsSprinting = false;
	}
	if(!HasAuthority())
	{
		ServerEnhancedSprint();
	}
}

void APlayerCharacter::EnhancedPickup()
{
	if (InventoryComp)
	{
		InventoryComp->AddToInventory(ItemHitLastFrame);
	}
}

void APlayerCharacter::EnhancedWeaponOne()
{
	if (InventoryComp)
	{
		InventoryComp->EquipWeaponOne();
	}
}

void APlayerCharacter::EnhancedWeaponTwo()
{
	if (InventoryComp)
	{
		InventoryComp->EquipWeaponTwo();
	}
}

void APlayerCharacter::EnhancedWeaponThree()
{
	if (InventoryComp)
	{
		InventoryComp->EquipWeaponThree();
	}
}

void APlayerCharacter::CalculateSpeed(float Value)
{
	if (bIsSprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = Value * 3.f + 450.f * 1.25f;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = Value * 3.f + 450.f;
	}
	if(!HasAuthority())
	{
		ServerCalculateSpeed();
	}
	
}

void APlayerCharacter::ServerCalculateSpeed_Implementation()
{
	CalculateSpeed(GetSpeed());
}

void APlayerCharacter::ServerEnhancedSprint_Implementation()
{
	EnhancedSprint();
}
