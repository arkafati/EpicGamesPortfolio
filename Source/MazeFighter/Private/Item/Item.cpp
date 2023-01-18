// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Item.h"
#include "Characters/Player/PlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"

AItem::AItem(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	bAlwaysRelevant = true;

	//Create Box collision that will be inherited for all items
	ItemCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("ItemCollision"));
	ItemCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ItemCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	ItemCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	ItemCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	//Create Spherical Area, in which you will be able to interact with objects, (inherited by items)
	ItemArea = CreateDefaultSubobject<USphereComponent>(TEXT("ItemArea"));
	ItemArea->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ItemArea->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	ItemArea->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetVisibility(false);

}




// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		ItemCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ItemCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		ItemArea->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ItemArea->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

		ItemArea->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
		ItemArea->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);
	}

}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
		if (PlayerCharacter)
		{
			PlayerCharacter->AddItemToPickup(this);
		}
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
		if (PlayerCharacter)
		{
			PlayerCharacter->SubtractItemToPickup(this);
		}
	}
}
