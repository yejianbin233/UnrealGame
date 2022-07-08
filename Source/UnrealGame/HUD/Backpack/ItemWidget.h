// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemInfoObject.h"
#include "Blueprint/UserWidget.h"
#include "ItemWidget.generated.h"

/**
 * 
 */
UCLASS()
class UNREALGAME_API UItemWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, Category="Widget", meta=(BindWidget), DisplayName="物品尺寸框")
	class USizeBox* ItemSizeBox;
	
	UPROPERTY(BlueprintReadWrite, Category="Widget", meta=(BindWidget), DisplayName="物品图像")
	class UImage* ItemImage;

	UPROPERTY(BlueprintReadWrite, Category="Widget", meta=(BindWidget), DisplayName="物品数量")
	class UTextBlock* ItemNum;

	UPROPERTY(BlueprintReadWrite, Category="Widget", meta=(BindWidget), DisplayName="物品边界")
	class UBorder* ItemBorder;

public:

	/*
	 *	当在执行拖动检测时：
	 *	1. 创建拖动操作控件，实现拖动鼠标
	 *	2. 在拖动时移除背包中的被拖动物体
	 */ 
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, DisplayName="拖动检测时的处理")
	void DragDetectedHandle();

	/*
	 * @description: CreateItemInfoObject - 创建可用于拖动控件的负载数据
	 * @param BackpackItemInfo - 背包物品数据
	 * 
	 * @return 拖动控件的负载数据，数据内容与 BackpackItemInfo 一样
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月08日 星期五 20:07:15
	 */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, DisplayName="创建拖动负载")
	UItemInfoObject* CreateItemInfoObject(FBackpackItemInfo BackpackItemInfo);
	
protected:

private:
	
	
};
