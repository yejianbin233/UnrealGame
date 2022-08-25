// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "CameraWidget.generated.h"

/**
 * 
 */
UCLASS()
class UNREALGAME_API UCameraWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Camera Material", DisplayName="摄像机聚焦盒子材质实例")
	UMaterialInstance* CameraFocusBoxMaterialInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Camera Material", DisplayName="摄像机噪点背景材质实例")
	UMaterialInstance* CameraNoiseMaterialInstance;
	UPROPERTY(BlueprintReadWrite, Category="Camera Material", DisplayName="摄像机噪点背景动态材质实例")
	UMaterialInstanceDynamic*  CameraNoiseMaterialInstanceDynamic;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Camera Material", DisplayName="摄像机闪光灯材质实例")
	UMaterialInstance* CameraFlashMaterialInstance;
	UPROPERTY(BlueprintReadWrite, Category="Camera Material", DisplayName="摄像机闪光灯动态材质实例")
	UMaterialInstanceDynamic*  CameraFlashMaterialInstanceDynamic;

	UPROPERTY(meta=(BindWidget))
	UImage* CameraFocusBox;
	UPROPERTY(meta=(BindWidget))
	UImage* Background;
	UPROPERTY(meta=(BindWidget))
	UImage* Foreground;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Camera Nosie", DisplayName="摄像机噪点像素大小名称")
	FName NoisePixelSizeName = TEXT("NoisePixelSize");
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Camera Nosie", DisplayName="摄像机噪点像素大小")
	float NoisePixelSize = 0.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Camera Nosie", DisplayName="摄像机噪点闪光速度名称")
	FName NoiseFlashSpeedName = TEXT("NoiseFlashSpeed");
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Camera Nosie", DisplayName="摄像机噪点闪光速度")
	float NoiseFlashSpeed = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Camera Flash", DisplayName="摄像机闪光灯闪光名称")
	FName FlashName = TEXT("Flash");
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Camera Flash", DisplayName="摄像机闪光灯闪光")
	float Flash = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Camera Flash", DisplayName="摄像机闪光遮罩名称")
	FName MaskName = TEXT("Mask");
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Camera Flash", DisplayName="摄像机闪光灯遮罩值")
	float Mask = 0.f;

public:
	
	virtual void NativePreConstruct() override;

	UFUNCTION(Category="Widget Init", DisplayName="摄像机控件初始化")
	void Init();

	UFUNCTION(Category="Camera Noise", DisplayName="更新噪点参数")
	void UpdateNoiseParam(float NewNoisePixelSize, float NewNoiseFlashSpeed);

	UFUNCTION(Category="Camera Flash", DisplayName="更新闪光参数")
	void UpdateFlash(float NewFlash, float NewMask);
};
