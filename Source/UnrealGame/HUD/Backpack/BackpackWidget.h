// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemOperatorWidget.h"
#include "ItemRenderWidget.h"
#include "Blueprint/UserWidget.h"
#include "UnrealGame/Backpack/ItemRender.h"
#include "BackpackWidget.generated.h"

/**
 * 
 */
UCLASS()
class UNREALGAME_API UBackpackWidget : public UUserWidget
{
	GENERATED_BODY()


public:

	UPROPERTY(BlueprintReadWrite, Category="Component")
	class UBackpackComponent* BackpackComponent;

	UPROPERTY(BlueprintReadWrite, Category="Widget", DisplayName="画布面板布局", meta=(BindWidget))
	class UCanvasPanel* CanvasPanel;

	UPROPERTY(BlueprintReadWrite, Category="Widget", DisplayName="物品渲染尺寸框", meta=(BindWidget))
	class USizeBox* ItenRenderSizeBox;
	
	UPROPERTY(BlueprintReadWrite, Category="Widget", DisplayName="背包边界", meta=(BindWidget))
	class UBorder* BackpackBorder;

	UPROPERTY(BlueprintReadWrite, Category="Widget", DisplayName="背包背景", meta=(BindWidget))
	class UBackgroundBlur* BackpackBackground;

	UPROPERTY(BlueprintReadWrite, Category="Widget", DisplayName="背包库存", meta=(BindWidget))
	class UInventoryWidget* InventoryWidget;

	UPROPERTY(EditAnywhere, Category="Widget", DisplayName="背包物品操作控件类", meta=(AllowPrivateAccess))
	TSubclassOf<UItemOperatorWidget> ItemOperatorWidgetClass;

	UPROPERTY(BlueprintReadWrite, Category="Widget", DisplayName="背包物品操作控件", meta=(AllowPrivateAccess))
	class UItemOperatorWidget* ItemOperatorWidget;
	
	UPROPERTY(EditAnywhere, Category="Widget", DisplayName="背包物品渲染控件类", meta=(AllowPrivateAccess))
	TSubclassOf<UItemRenderWidget> ItemRenderWidgetClass;

	UPROPERTY(BlueprintReadWrite, Category="Widget", DisplayName="背包物品渲染控件", meta=(AllowPrivateAccess))
	class UItemRenderWidget* ItemRenderWidget;

	UPROPERTY(EditAnywhere, Category="Widget", DisplayName="背包物品渲染类", meta=(AllowPrivateAccess))
	TSubclassOf<class AItemRender> ItemRenderClass;
	
	UPROPERTY(BlueprintReadWrite, Category="Item Render", DisplayName="物品渲染Actor")
	AItemRender* ItemRender;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Widget", DisplayName="背包物品渲染控件尺寸", meta=(AllowPrivateAccess))
	FVector2D RenderWidgetSize;
	
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

	UFUNCTION(Client, Reliable, BlueprintCallable, Category="Item Render", DisplayName="创建物品渲染Actor")
	void CreateItemRender();

	UFUNCTION(Client, Reliable, BlueprintCallable, Category="Item Render", DisplayName="是否显示物品渲染界面")
	void ShowItemRender(bool bIsShow);
	
protected:

	

private:
	
};
