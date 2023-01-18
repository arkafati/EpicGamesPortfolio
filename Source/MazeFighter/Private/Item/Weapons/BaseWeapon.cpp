// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapons/BaseWeapon.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/Player/PlayerCharacter.h"
#include "AbilitySystemComponent.h"
#include "Characters/Components/CombatComponent.h"

ABaseWeapon::ABaseWeapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	bAlwaysRelevant = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	ItemCollision->SetupAttachment(RootComponent);
	ItemArea->SetupAttachment(RootComponent);
	PickupWidget->SetupAttachment(RootComponent);

	APlayerCharacter* Character = Cast<APlayerCharacter>(GetOwner());

	bCanPrimaryAttack = true;
}

void ABaseWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseWeapon, WeaponState);
	DOREPLIFETIME(ABaseWeapon, PrimaryAttack);
	DOREPLIFETIME(ABaseWeapon, SecondaryAttack);
}

EWeaponState ABaseWeapon::GetWeaponState()
{
	return WeaponState;
}

EPrimaryAttackType ABaseWeapon::GetPrimaryAttackType()
{
	return PrimaryAttack;
}

ESecondaryAttackType ABaseWeapon::GetSecondaryAttackType()
{
	return SecondaryAttack;
}

void ABaseWeapon::SetWeaponState(EWeaponState State)
{
	WeaponState = State;
	OnWeaponState();
}

void ABaseWeapon::OnWeaponState()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		ItemCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		break;

	case EWeaponState::EWS_InInventory:
		Mesh->SetSimulatePhysics(false);
		Mesh->SetEnableGravity(false);
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetPickupWidget()->SetVisibility(false);
		ItemCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ItemArea->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;

	case EWeaponState::EWS_Dropped:
		Mesh->SetSimulatePhysics(false);
		Mesh->SetEnableGravity(false);
		Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ItemCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ItemCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		ItemArea->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ItemArea->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		break;
	}
}

void ABaseWeapon::OnRep_WeaponState()
{
	OnWeaponState();
}

void ABaseWeapon::ApplyDamage(FHitResult& HitResult)
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(HitResult.GetActor());
	if (PlayerCharacter && PlayerCharacter->GetAbilitySystemComponent())
	{
		if (HasAuthority())
		{
			FGameplayEffectContextHandle EffectContext = PlayerCharacter->GetAbilitySystemComponent()->MakeEffectContext();
			EffectContext.AddSourceObject(PlayerCharacter);

			FGameplayEffectSpecHandle NewHandle = PlayerCharacter->GetAbilitySystemComponent()->MakeOutgoingSpec(DamageGameplayEffect, 1, EffectContext);
			if (NewHandle.IsValid())
			{
				NewHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), BaseDamage);
				FActiveGameplayEffectHandle ActiveGEHandle = PlayerCharacter->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), PlayerCharacter->GetAbilitySystemComponent());
			}
		}
	}
}

void ABaseWeapon::HitScanFire(const FVector& HitTarget)
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstigatorController = OwnerPawn->GetController();

	const USkeletalMeshSocket* MuzzleFlashSocket = Mesh->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket && InstigatorController)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(Mesh);
		FVector Start = SocketTransform.GetLocation();
		FVector End = Start + (HitTarget - Start) * 1.25f;

		FHitResult FireHit;
		UWorld* World = GetWorld();
		if (World)
		{
			World->LineTraceSingleByChannel(
				FireHit,
				Start,
				End,
				ECollisionChannel::ECC_Visibility
			);
			if (FireHit.bBlockingHit)
			{
				ApplyDamage(FireHit);
			}
		}
	}
}
