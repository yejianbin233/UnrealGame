// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Engine/DataTable.h"
#include "ItemInfoObject.h"
#include "InventoryWidget.generated.h"


USTRUCT(BlueprintType)
struct FLink : public FTableRowBase 
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="Link")
	FVector2D Start;
	UPROPERTY(BlueprintReadWrite, Category="Link")
	FVector2D End;
	
};

/**
 * 
 */
UCLASS()
class UNREALGAME_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, Category="Component")
	class UBackpackComponent* BackpackComponent;

	UPROPERTY(BlueprintReadWrite, Category="Widget", DisplayName="背包边界框", meta=(BindWidget))
	UCanvasPanel* BackpackBorder;

	UPROPERTY(BlueprintReadWrite, Category="Widget", DisplayName="背包内容上下文框", meta=(BindWidget))
	UCanvasPanel* BackpackContent;

	UPROPERTY(BlueprintReadWrite, Category="Widget", DisplayName="背包背景", meta=(BindWidget))
	UBorder* BackpackBackground;

	UPROPERTY(BlueprintReadWrite, Category="Properties", DisplayName="背包绘制的线条数组")
	TArray<FLink> Links;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Properties", DisplayName="物品格子控件类")
	TSubclassOf<class UItemWidget> ItemWidgetClass;

	UPROPERTY(BlueprintReadWrite, Category="Properties", DisplayName="格子大小")
	float CellSize;

	UPROPERTY(BlueprintReadWrite, Category="Drag", DisplayName="是否需要绘制放置提示框")
	bool bIsNeedDrawPlaceTipsBox;

	UPROPERTY(BlueprintReadWrite, Category="Drag", DisplayName="待绘制提示框是否在背包有效区域内")
	bool bIsInBackpackValidArea;

	UPROPERTY(BlueprintReadWrite, Category="Drag", DisplayName="是否可放置拖动物品")
	bool bIsCanPlaceDragItem;
	
	UPROPERTY(BlueprintReadWrite, Category="Drag", DisplayName="绘制放置提示框的位置")
	FVector2D DrawPlaceTipsBoxPosition;

	UPROPERTY(BlueprintReadWrite, Category="Drag", DisplayName="绘制放置提示框的大小")
	FVector2D DrawPlaceTipsBoxSize;
	
	UPROPERTY(BlueprintReadWrite, Category="Drag", DisplayName="绘制放置提示框的笔刷资产")
	USlateBrushAsset* TipsBoxBrushAsset;
	
public:
	/*
	 * @description: Init - 初始化背包库存控件
	 * @param InBackpackComponent - 背包组件
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月08日 星期五 20:07:20
	 */
	UFUNCTION(BlueprintCallable, Category="Backpack Function")
	void Init(UBackpackComponent* InBackpackComponent);

	/*
	 * @description: SetBackpackBorderSize - 设置背包库存边界大小
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月08日 星期五 20:07:20
	 */
	UFUNCTION(BlueprintCallable, Category="Backpack Function")
	void SetBackpackBorderSize();

	/*
	 * @description: InitLinks - 根据背包数据初始化线条数组
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月08日 星期五 20:07:20
	 */
	UFUNCTION(BlueprintCallable, Category="Backpack Function")
	void InitLinks();

	/*
	 * @description: Refresh - 刷新背包库存数据
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月08日 星期五 20:07:20
	 */
	UFUNCTION(BlueprintCallable, Category="Backpack Function", DisplayName="刷新背包")
	void Refresh();

	/*
	 * @description: CreateItemWidget - 用于创建可放置在背包库存中的背包物品控件
	 * @param InPosition - 物品控件放置的位置
	 * @param FBackpackItemInfo - 物品数据
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月08日 星期五 20:07:20
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Backpack Function", DisplayName="创建物品格控件")
	UItemWidget* CreateItemWidget(FVector2D InPosition, struct FBackpackItemInfo InItemInfo);

	/*
	 *	DragOverHandle - 用于背包控件及子控件(物品控件)在拖动物品控件时的拖动处理
	 *
	 *	在拖动时，根据拖动鼠标的位置判断拖动物品控件底下位于背包的那一格，在其他绘制方框以辅助玩家放置物品
	 *
	 *	MouseAbsolutePosition - 鼠标位置在屏幕空间上的绝对位置
	 *
	 *	note：TODO 已蓝图实现
	 */
	// UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintCosmetic, DisplayName="拖动处理")
	// void DragOverHandle(FVector2D MouseAbsolutePosition);

	
	/*
	 * @description: IsValidBackpackArea - 判断是否将物品拖出背包的有效空间外
	 * @param LeftTopPoint - 左上角位置
	 * @param RightBottomPoint - 右下角位置
	 * @param EdgeSize - 边缘大小，额外边缘用作容差
	 * 
	 * @return 结果
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月08日 星期五 20:07:13
	 */
	UFUNCTION(BlueprintCallable, DisplayName="为背包控件有效空间")
	bool IsValidBackpackArea(FVector2D LeftTopPoint, FVector2D RightBottomPoint, float EdgeSize);

	
};
