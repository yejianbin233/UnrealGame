// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BackpackWidget.generated.h"

/**
 * 
 */
UCLASS()
class UNREALGAME_API UBackpackWidget : public UUserWidget
{
	GENERATED_BODY()


public:

	UPROPERTY(BlueprintReadWrite, Category="Widget", DisplayName="背包边界", meta=(BindWidget))
	class UBorder* BackpackBorder;

	UPROPERTY(BlueprintReadWrite, Category="Widget", DisplayName="背包背景", meta=(BindWidget))
	class UBackgroundBlur* BackpackBackground;

	UPROPERTY(BlueprintReadWrite, Category="Widget", DisplayName="背包库存", meta=(BindWidget))
	class UInventoryWidget* InventoryWidget;


public:
	/*
	 * @description: RotateDragItemWidget - 旋转正在拖动中的物品UI控件
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月08日 星期五 20:07:59
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="背包控件", DisplayName="旋转拖放物品控件")
	void RotateDragItemWidget();
	
protected:

	

private:
	
};
