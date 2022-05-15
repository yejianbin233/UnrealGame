// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealGame/HUD/BlasterHUD.h"
#include "UnrealGame/HUD/CharacterOverlay.h"

void ABlasterHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D ViewportSize;
	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);

		const FVector2D ViewportCenter(ViewportSize.X / 2.0f, ViewportSize.Y / 2.0f);

		float SpreadScale = CrosshairSpreadMax * HudPackage.CrosshairSpread;
		if (HudPackage.CrosshairsCenter)
		{
			FVector2D Spread(0.0f, 0.0f);
			DrawCrosshair(HudPackage.CrosshairsCenter, ViewportCenter, Spread, HudPackage.CrosshairsColor);
		}
		if (HudPackage.CrosshairsLeft)
		{
			FVector2D Spread(-SpreadScale, 0.0f);
			DrawCrosshair(HudPackage.CrosshairsLeft, ViewportCenter, Spread, HudPackage.CrosshairsColor);
		}
		if (HudPackage.CrosshairsRight)
		{
			FVector2D Spread(SpreadScale, 0.0f);
			DrawCrosshair(HudPackage.CrosshairsRight, ViewportCenter, Spread, HudPackage.CrosshairsColor);
		}
		if (HudPackage.CrosshairsTop)
		{
			FVector2D Spread(0.0f, -SpreadScale);
			DrawCrosshair(HudPackage.CrosshairsTop, ViewportCenter, Spread, HudPackage.CrosshairsColor);
		}
		if (HudPackage.CrosshairsBottom)
		{
			FVector2D Spread(0.0f, SpreadScale);
			DrawCrosshair(HudPackage.CrosshairsBottom, ViewportCenter, Spread, HudPackage.CrosshairsColor);
		}

		
	}
}

void ABlasterHUD::BeginPlay()
{
	Super::BeginPlay();

	AddCharacterOverlay();
}

void ABlasterHUD::AddCharacterOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();

	if (PlayerController && CharacterOverlayClass)
	{
		CharacterOverlay = CreateWidget<UCharacterOverlay>(PlayerController, CharacterOverlayClass);

		CharacterOverlay->AddToViewport();
	}
}

void ABlasterHUD::DrawCrosshair(UTexture2D* Texture2D, FVector2D ViewportLocation, FVector2D Spread, FLinearColor CrosshairsColor)
{
	const float TextureWidth = Texture2D->GetSizeX();
	const float TextureHeight = Texture2D->GetSizeY();

	const FVector2D TextureDrawPoint(
		ViewportLocation.X - (TextureWidth / 2.0f) + Spread.X
		, ViewportLocation.Y - (TextureWidth / 2.0f) + Spread.Y
	);

	DrawTexture(Texture2D
		, TextureDrawPoint.X
		, TextureDrawPoint.Y
		, TextureWidth
		, TextureHeight
		, 0.
		, 0.0f
		, 1.0f
		, 1.0f
		, CrosshairsColor
		);
}
