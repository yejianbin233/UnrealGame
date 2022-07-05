// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UnrealGame/Component/Collimation/CrosshairComponent.h"
#include "BlasterHUD.generated.h"


/**
 * 
 */
UCLASS()
class UNREALGAME_API ABlasterHUD : public AHUD
{
	GENERATED_BODY()

public:

	virtual void DrawHUD() override;

	UPROPERTY(EditAnywhere, Category="Player State")
	TSubclassOf<class UUserWidget> CharacterOverlayClass;

	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;
	
	void AddCharacterOverlay();

	UPROPERTY(EditAnywhere, Category = "Announcement")
	TSubclassOf<UUserWidget> AnnouncementClass;

	UPROPERTY()
	class UAnnouncement* Announcement;

	void AddAnnouncement();
	
protected:

	virtual void BeginPlay() override;

private:

	FCrosshairHUDPackage HudPackage;

	void DrawCrosshair(UTexture2D* Texture2D, FVector2D ViewportLocation, FVector2D Spread, FLinearColor CrosshairsColor);

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess=true))
	float CrosshairSpreadMax = 16.0f;

public:

	FORCEINLINE void SetHUDPackage(const FCrosshairHUDPackage& Package) { HudPackage = Package;};
};
