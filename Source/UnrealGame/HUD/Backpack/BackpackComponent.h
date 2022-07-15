// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemInfoObject.h"
#include "Components/ActorComponent.h"
#include "UnrealGame/Character/BlasterCharacter.h"
#include "BackpackComponent.generated.h"

USTRUCT(BlueprintType)
struct FPositionItem
{
	GENERATED_BODY()

	FVector2D Position;

	FBackpackItemInfo* Item;
};

DECLARE_EVENT(UBackpackComponent, FOnBackpackItemChanged)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType )
class UNREALGAME_API UBackpackComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, Category="Reference", DisplayName="玩家控制器")
	class APlayerController* PlayerController;

	UPROPERTY(BlueprintReadOnly, Category="Reference", DisplayName="玩家角色")
	class ABlasterCharacter* BlasterCharacter;

	UPROPERTY(BlueprintReadOnly, Category="Widget", DisplayName="背包控件")
	class UBackpackWidget* BackpackWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget", DisplayName="背包仓库控件")
	class UInventoryWidget* BackpackInventoryWidget;

	UPROPERTY(EditAnywhere, Category="Widget", DisplayName="背包控件类")
	TSubclassOf<class UUserWidget> BackpackWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Backpack Setting", DisplayName="背包行数")
	int Row;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Backpack Setting", DisplayName="背包列数")
	int Column;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Backpack Setting", DisplayName="背包单元格大小")
	float CellSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category="Backpack Setting", DisplayName="背包物品")
	TArray<FBackpackItemInfo> Items;

	// 背包数据改变时事件，通知 UI 更新
	FOnBackpackItemChanged OnBackpackItemChanged;
	
public:	
	// Sets default values for this component's properties
	UBackpackComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	/*
	 * @description:TryAddItem - 尝试添加物品，拾取物品时调用的背包添加物品方法
	 * @param FBackpackItemInfo - 背包存储的物品数据结构
	 *	note：传引用是为了部分添加，剩余未添加的物品修改其数据
	 * 
	 * @return 返回添加结果
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月08日 星期五 19:07:51
	 */
	UFUNCTION(BlueprintCallable, Category="背包组件", DisplayName="尝试添加物品")
	bool TryAddItem(FBackpackItemInfo& Item);

	/*
	 * @description: TryInsertItem - 尝试插入物品到背包指定位置，拖动物品控件时调用的背包放置物品方法（因为在拖动时会删除背包中的物品，因此是指定添加）
	 * @param FBackpackItemInfo - 背包存储的物品数据结构
	 * @param Index - 指定在背包中的数组下标
	 * 
	 * @return 返回添加结果
	 * ...
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月08日 星期五 19:07:33
	 */
	UFUNCTION(BlueprintCallable, Category="背包组件", DisplayName="尝试插入物品")
	bool TryInsertItem(FBackpackItemInfo Item, int Index);

	/*
	 * @description: IsHadPlace - 背包是否有空间放置物品
	 * @param FBackpackItemInfo - 背包存储的物品数据结构
	 * @param Index - 指定在背包中的数组下标
	 * 
	 * @return 返回处理结果
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月08日 星期五 20:07:36
	 *
	 * * TODO 修改了参入参数
	 */
	bool IsHadPlace(FBackpackItemInfo Item, int* Index);

	/*
	 * @description: PlaceIndexCheck - 背包指定位置是否有空间放置物品
	 * @param FBackpackItemInfo - 背包存储的物品数据结构
	 * @param Index - 指定在背包中的数组下标
	 * 
	 * @return 返回处理结果
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月08日 星期五 20:07:36
	 *
	 * TODO 修改了参入参数
	 */
	UFUNCTION(BlueprintCallable, Category="背包组件", DisplayName="检测指定位置是否可放置物品")
	bool PlaceIndexCheck(FBackpackItemInfo Item, int Index);

	// TODO 可删除函数 - K2_PlaceIndexCheck
	// bool K2_PlaceIndexCheck(FBackpackItemInfo Item, int Index);

	/*
	 * @description: CoordinateConvert - 格子坐标与数组索引之间的相互转换
	 * @param Index - 数组索引
	 * @param Position - 格子坐标位置
	 * @param bToIndex - 是否转换为数组索引
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月08日 星期五 20:07:20
	 */
	// UFUNCTION(BlueprintCallable, Category="背包组件", DisplayName="格子坐标与数组索引之间的相互转换")
	void CoordinateConvert(int* Index, FVector2D* Position, bool* bToIndex);

	/*
	 * 
	 */
	/*
	 * @description: PackpackCoordinateConvert - 背包屏幕本地坐标转换为格子坐标
	 * @param BackpackWidgetLocalPosition - 背包控件本地坐标位置
	 * @param Position - 格子坐标位置
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月08日 星期五 20:07:20
	 */
	// UFUNCTION(BlueprintCallable, Category="背包组件", DisplayName="背包屏幕本地坐标转换为格子坐标")
	void PackpackCoordinateConvert(FVector2D BackpackWidgetLocalPosition, FVector2D& Position);

	/*
	 * @description: GetIndexByBackpackLocalPosition - 根据背包屏幕本地坐标获取对应的数组索引
	 * @param BackpackWidgetLocalPosition - 背包控件本地坐标位置
	 * @return 背包数组索引
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月08日 星期五 20:07:08
	 */
	UFUNCTION(BlueprintCallable, Category="背包组件", DisplayName="根据背包屏幕本地坐标获取对应的数组索引")
	int GetIndexByBackpackLocalPosition(FVector2D BackpackWidgetLocalPosition);

	/*
	 * @description: GetBackpackLocalPositionByIndex - 根据数组索引获取在背包中的本地位置
	 * @param int - 背包数组索引
	 * @return 格子坐标位置
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月08日 星期五 20:07:08
	 */
	UFUNCTION(BlueprintCallable, Category="背包组件", DisplayName="根据数组索引获取在背包中的本地位置")
	FVector2D GetBackpackLocalPositionByIndex(int Index);

	/*
	* @description:PlaceBackpackTipsBoxByIndex - 提示框具有长宽，而鼠标位置的索引是提示框的中心，因此需要根据提示框的长宽来偏移，得到在背包内的正确放置位置
	*	@param InIndex - 鼠标位置所在的数组索引
	*	@param OccupyCellXYLength - 提示框所占据的背包格子
	* @return 实际应该放置的背包数组索引
	* ...
	*
	* note：使用的前提时，提示框在背包的有效范围内，并且不超过冗余的边缘
	* 
	* @author: yejianbin
	* @version: v1.0
	* @createTime: 2022年07月08日 星期五 09:07:46
	*/
	UFUNCTION(BlueprintCallable, Category="背包组件", DisplayName="获取放置背包范围内的提示框放置索引")
	int PlaceBackpackTipsBoxByIndex(int InIndex, FIntPoint OccupyCellXYLength);

	/*
	 * @description: GetAllItem - 获取背包所有物品数据
	 * @param PositionItems - 用于存储背包物品的数组
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月08日 星期五 20:07:43
	 */
	void GetAllItem(TArray<FPositionItem>* PositionItems);

	/*
	 * @description: GetItems - 根据物品ID获取指定的背包物品
	 * @param PositionItems - 用于存储背包物品的数组
	 *	@param Id - 物品 ID
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月08日 星期五 20:07:43
	 */
	void GetItems(TArray<FPositionItem>* PositionItems, FString Id);

	/*
	 * @description: IsValidPosition - 判断是否是有效的格子
	 * @param Position - 背包格子
	 * @return 结果
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月08日 星期五 20:07:29
	 */
	bool IsValidPosition(FVector2D Position);

	/*
	 * @description: OpenOrCloseBackpack - 打开或关闭背包UI控件
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月08日 星期五 20:07:05
	 */
	UFUNCTION(BlueprintCallable, Category="BackpackWidget", DisplayName="打开或关闭背包控件")
	void OpenOrCloseBackpack();

	/*
	 * @description: RemoveItem - 移除指定 BackpackId 的物品
	 * @param BackpackId - 物品在背包中的唯一 Id，同等于数组在背包数组中的下标索引
	 * @return 结果
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月08日 星期五 20:07:22
	 */
	bool RemoveItem(FString BackpackId);

	/*
	 * @description: TryRemoveItem - 尝试移除背包物品
	 * @param BackpackId - 物品在背包中的唯一 Id，同等于数组在背包数组中的下标索引
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月08日 星期五 20:07:41
	 */
	UFUNCTION(BlueprintCallable, Category="背包组件", DisplayName="尝试移除背包物品")
	void TryRemoveItem(FString BackpackId);

	/*
	 * @description: CreateItemAfterDiscard - 在丢弃背包物品时，在场景中生成物品 Actor
	 * @param FString - 物品的 Id
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月08日 星期五 20:07:07
	 */
	UFUNCTION(BlueprintCallable, Category="背包组件", DisplayName="丢弃物品后在场景中生成物品")
	void CreateItemAfterDiscard(FString Id);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	/*
	 * @description: AddNewItem - 在背包指定位置放置物品
	 * @param Item - 物品
	 * @param Index - 背包位置
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月08日 星期五 20:07:07
	 */
	void AddNewItem(FBackpackItemInfo* Item, int Index);

	
		
};

