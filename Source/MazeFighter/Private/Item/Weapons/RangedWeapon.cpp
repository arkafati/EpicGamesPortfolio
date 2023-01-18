// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapons/RangedWeapon.h"
#include "Item/Item.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"

ARangedWeapon::ARangedWeapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	bAlwaysRelevant = true;

}

void ARangedWeapon::FireHitScan()
{
}

void ARangedWeapon::FireShotgun()
{
}
