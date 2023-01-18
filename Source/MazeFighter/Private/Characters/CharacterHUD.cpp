// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/CharacterHUD.h"
#include "GameFramework/PlayerController.h"
#include "Characters/BasePlayerController.h"
#include "Characters/UI/CharacterOverlay.h"

void ACharacterHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D ViewportSize;
	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	}
}

void ACharacterHUD::BeginPlay()
{
	Super::BeginPlay();

	AddCharacterOverlay();
}

void ACharacterHUD::AddCharacterOverlay()
{
	APlayerController* PC = GetOwningPlayerController();
	if (PC && CharacterOverlayClass)
	{
		CharacterOverlay = CreateWidget<UCharacterOverlay>(PC, CharacterOverlayClass);
		CharacterOverlay->AddToViewport();
	}
}
