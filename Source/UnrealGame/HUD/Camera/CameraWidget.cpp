// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraWidget.h"

void UCameraWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	Init();
}

void UCameraWidget::Init()
{
	if (CameraFocusBoxMaterialInstance)
	{
		CameraFocusBox->SetBrushFromMaterial(CameraFocusBoxMaterialInstance);
	}

	if (CameraNoiseMaterialInstance)
	{
		CameraNoiseMaterialInstanceDynamic = UMaterialInstanceDynamic::Create(CameraNoiseMaterialInstance, this);

		CameraNoiseMaterialInstanceDynamic->SetScalarParameterValue(NoisePixelSizeName, NoisePixelSize);
		CameraNoiseMaterialInstanceDynamic->SetScalarParameterValue(NoiseFlashSpeedName, NoiseFlashSpeed);
		Background->SetBrushFromMaterial(CameraNoiseMaterialInstanceDynamic);
	}

	if (CameraFlashMaterialInstance)
	{
		CameraFlashMaterialInstanceDynamic = UMaterialInstanceDynamic::Create(CameraFlashMaterialInstance, this);

		CameraFlashMaterialInstanceDynamic->SetScalarParameterValue(FlashName, Flash);
		CameraFlashMaterialInstanceDynamic->SetScalarParameterValue(MaskName, Mask);
		Foreground->SetBrushFromMaterial(CameraFlashMaterialInstanceDynamic);
	}
}

void UCameraWidget::UpdateNoiseParam(float NewNoisePixelSize, float NewNoiseFlashSpeed)
{
	CameraNoiseMaterialInstanceDynamic->SetScalarParameterValue(NoisePixelSizeName, NewNoisePixelSize);
	CameraNoiseMaterialInstanceDynamic->SetScalarParameterValue(NoiseFlashSpeedName, NewNoiseFlashSpeed);
}

void UCameraWidget::UpdateFlash(float NewFlash, float NewMask)
{
	CameraFlashMaterialInstanceDynamic->SetScalarParameterValue(FlashName, NewFlash);
	CameraFlashMaterialInstanceDynamic->SetScalarParameterValue(MaskName, NewMask);
}


