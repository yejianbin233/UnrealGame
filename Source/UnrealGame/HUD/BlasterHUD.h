// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BlasterHUD.generated.h"

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()

public:
	class UTexture2D* CrosshairsCenter;
	class UTexture2D* CrosshairsTop;
	class UTexture2D* CrosshairsLeft;
	class UTexture2D* CrosshairsBottom;
	class UTexture2D* CrosshairsRight;
	float CrosshairSpread;
	FLinearColor CrosshairsColor;
};
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

	FHUDPackage HudPackage;

	void DrawCrosshair(UTexture2D* Texture2D, FVector2D ViewportLocation, FVector2D Spread, FLinearColor CrosshairsColor);

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess=true))
	float CrosshairSpreadMax = 16.0f;

public:

	FORCEINLINE void SetHUDPackage(const FHUDPackage& Package) { HudPackage = Package;};
};
