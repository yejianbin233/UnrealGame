// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AimComponent.h"
#include "CameraAimComponent.generated.h"

/**
 * 
 */
UCLASS()
class UNREALGAME_API UCameraAimComponent : public UAimComponent
{
	GENERATED_BODY()

	virtual void WidgetAim(bool bToAim) override;
};
