// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CrosshairWidget.generated.h"

/**
 * 
 */
UCLASS()
class UNREALGAME_API UCrosshairWidget : public UUserWidget
{
	GENERATED_BODY()


public:

	UPROPERTY(meta=(BindWidget))
	class UImage* CrosshairsCenter;

	UPROPERTY(meta=(BindWidget))
	class UImage* CrosshairsTop;

	UPROPERTY(meta=(BindWidget))
	class UImage* CrosshairsLeft;

	UPROPERTY(meta=(BindWidget))
	class UImage* CrosshairsBottom;

	UPROPERTY(meta=(BindWidget))
	class UImage* CrosshairsRight;
};
