// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	EIT_None		UMETA(DisplayName = "None"),
	EIT_Ammo		UMETA(DisplayName = "Ammo"),
	EIT_Weapon		UMETA(DisplayName = "Weapon"),
	EIT_Pickup		UMETA(DisplayName = "Pickup"),

	EIT_MAX			UMETA(DisplayName = "DefaultMax")
};
UCLASS()
class MAZEFIGHTER_API AItem : public AActor
{
	GENERATED_BODY()
	
public:
	AItem(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Mesh")
	class UBoxComponent* ItemCollision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Mesh")
	class USphereComponent* ItemArea;
	UPROPERTY(EditAnywhere, Category = "Weapon|Mesh")
	class UWidgetComponent* PickupWidget;
	

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
		virtual void OnSphereOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);
	UFUNCTION()
		virtual void OnSphereEndOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);

public:
	FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidget; }
};
