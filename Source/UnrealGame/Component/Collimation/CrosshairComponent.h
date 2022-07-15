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

	UPROPERTY(BlueprintReadWrite, DisplayName="准心中心")
	class UTexture2D* CrosshairsCenter;

	UPROPERTY(BlueprintReadWrite, DisplayName="准心顶部")
	class UTexture2D* CrosshairsTop;
	
	UPROPERTY(BlueprintReadWrite, DisplayName="准心左边")
	class UTexture2D* CrosshairsLeft;

	UPROPERTY(BlueprintReadWrite, DisplayName="准心底部")
	class UTexture2D* CrosshairsBottom;

	UPROPERTY(BlueprintReadWrite, DisplayName="准心右边")
	class UTexture2D* CrosshairsRight;

	UPROPERTY(BlueprintReadWrite, DisplayName="准心扩大量")
	float CrosshairSpread;

	UPROPERTY(BlueprintReadWrite, DisplayName="准心颜色值")
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
	
public:

	UPROPERTY(BlueprintReadOnly, Category="Reference", DisplayName="玩家角色")
	class ABlasterCharacter* BlasterCharacter;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Widget", DisplayName="准星UI控件类")
	TSubclassOf<class UUserWidget> CrosshairWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Widget", DisplayName="准星UI控件")
	class UCrosshairWidget* CrosshairWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI | Crosshairs", DisplayName="准心扩大曲线资产")
	UCurveFloat* CrosshairOffsetCurve;
	
	// 准心由 5 个部分组成
	// 纹理压缩设置"用户界面2D"
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "UI | Crosshairs", DisplayName="准心中心")
	class UTexture2D* CrosshairsCenter;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "UI | Crosshairs", DisplayName="准心顶部")
	class UTexture2D* CrosshairsTop;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "UI | Crosshairs", DisplayName="准心左边")
	class UTexture2D* CrosshairsLeft;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "UI | Crosshairs", DisplayName="准心底部")
	class UTexture2D* CrosshairsBottom;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "UI | Crosshairs", DisplayName="准心右边")
	class UTexture2D* CrosshairsRight;
	
};
