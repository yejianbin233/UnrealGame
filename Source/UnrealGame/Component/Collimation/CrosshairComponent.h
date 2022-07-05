// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CollimationComponent.h"
#include "CrosshairComponent.generated.h"


USTRUCT(BlueprintType)
struct FCrosshairHUDPackage
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
class UNREALGAME_API UCrosshairComponent : public UCollimationComponent
{
	GENERATED_BODY()

	UCrosshairComponent();

public:
	virtual void ShowCollimation() override;
	virtual void HideCollimation() override;;
	virtual void UpdateSpread() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
private:

	class ABlasterCharacter* BlasterCharacter;
	
	UPROPERTY(EditAnywhere, Category="Player State")
	TSubclassOf<class UUserWidget> CrosshairWidgetClass;

	class UCrosshairWidget* CrosshairWidget;

	// UPROPERTY(EditDefaultsOnly, Category = "UI | Crosshairs")
	// FFloatCurve CrosshairOffsetCurve;
	
	UPROPERTY(EditDefaultsOnly, Category = "UI | Crosshairs")
	UCurveFloat* CrosshairOffsetCurve;
	
	// 准心由 5 个部分组成
	// 纹理压缩设置"用户界面2D"
	UPROPERTY(EditDefaultsOnly, Category = "UI | Crosshairs")
	class UTexture2D* CrosshairsCenter;

	UPROPERTY(EditDefaultsOnly, Category = "UI | Crosshairs")
	class UTexture2D* CrosshairsTop;

	UPROPERTY(EditDefaultsOnly, Category = "UI | Crosshairs")
	class UTexture2D* CrosshairsLeft;

	UPROPERTY(EditDefaultsOnly, Category = "UI | Crosshairs")
	class UTexture2D* CrosshairsBottom;
	
	UPROPERTY(EditDefaultsOnly, Category = "UI | Crosshairs")
	class UTexture2D* CrosshairsRight;
};
