// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemInfoObject.h"
#include "Components/ActorComponent.h"
#include "UnrealGame/Character/BlasterCharacter.h"
#include "BackpackComponent.generated.h"
/*
 * FPositionItem - 背包物品位置
 */
USTRUCT(BlueprintType)
struct FPositionItem
{
	GENERATED_BODY()

	FVector2D Position;

	FBackpackItemInfo* Item;
};

/* FOnBackpackItemChanged - 背包物品改变委托 */
DECLARE_EVENT(UBackpackComponent, FOnBackpackItemChanged)

DECLARE_EVENT_OneParam(UBackpackComponent, FOnServerPickupItemFailture, AItemBase*)


//TODO 背包设计 - 多人网络改造
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType )
class UNREALGAME_API UBackpackComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category="Reference", DisplayName="玩家控制器", meta=(AllowPrivateAccess))
	class APlayerController* PlayerController;

	UPROPERTY(BlueprintReadOnly, Category="Reference", DisplayName="玩家角色", meta=(AllowPrivateAccess))
	class ABlasterCharacter* PlayerCharacter;

	UPROPERTY(BlueprintReadOnly, Category="Widget", DisplayName="背包控件", meta=(AllowPrivateAccess))
	class UBackpackWidget* BackpackWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget", DisplayName="背包仓库控件", meta=(AllowPrivateAccess))
	class UInventoryWidget* BackpackInventoryWidget;

	UPROPERTY(EditAnywhere, Category="Widget", DisplayName="背包控件类", meta=(AllowPrivateAccess))
	TSubclassOf<class UUserWidget> BackpackWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Backpack Setting", DisplayName="背包行数", meta=(AllowPrivateAccess))
	int Row;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Backpack Setting", DisplayName="背包列数", meta=(AllowPrivateAccess))
	int Column;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Backpack Setting", DisplayName="背包单元格大小", meta=(AllowPrivateAccess))
	float CellSize;

	// 背包数据改变时事件，通知 UI 更新
	FOnBackpackItemChanged OnBackpackItemChanged;

	FOnServerPickupItemFailture OnPickupItemFailture;

	FOnClientBackpackItemChanged OnClientBackpackItemChanged;
	
	FOnServerReportBackpackItemChanged OnServerReportBackpackItemChanged;
	
private:
	UPROPERTY(BlueprintReadWrite, Category="Backpack", DisplayName="背包存储的物品", meta=(AllowPrivateAccess))
	TArray<FBackpackItemInfo> Items;
	
public:	
	// Sets default values for this component's properties
	UBackpackComponent();

	/*
	 * @description: OpenOrCloseBackpack - 打开或关闭背包UI控件
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月08日 星期五 20:07:05
	 */
	UFUNCTION(Client, Reliable, BlueprintCallable, Category="BackpackWidget", DisplayName="打开或关闭背包控件")
	void OpenOrCloseBackpack();

	UFUNCTION(BlueprintCallable, Category="Backpack", DisplayName="客户端拾取")
	EPickupResult Pickup(AItemBase* PickItem);
	
	// UFUNCTION(Client, Reliable, BlueprintCallable, Category="Backpack", DisplayName="客户端拾取")
	// void CC_Pickup(AItemBase* PickItem);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Pickup", DisplayName="服务器拾取")
	void SC_Pickup(AItemBase* PickItem, float BackpackItemChangedTime);

	UFUNCTION(BlueprintCallable, Category="Pickup", DisplayName="服务器作为客户端拾取")
	void SNC_Pickup(AItemBase* PickItem);
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
	void TryInsertItem(FBackpackItemInfo Item, int Index);
	/*
	 * @description: GetAllItem - 获取背包所有物品数据
	 * @param PositionItems - 用于存储背包物品的数组
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月08日 星期五 20:07:43
	 */
	void GetAllItem(TArray<FPositionItem>& PositionItems);

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
	 * @description: GetItems - 根据背包ID获取指定的背包物品
	 * @param PositionItems - 用于存储背包物品的数组
	 *	@param BackpackId - 背包 ID
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月08日 星期五 20:07:43
	 */
	void GetItemsByBackpackId(TArray<FBackpackItemInfo>& BackpackItemInfos, FString BackpackId);
	/*
	 * @description: UpdateItemNum - 根据背包Id更新物品数量
	 * @param BackpackId - 物品在背包中的唯一 Id，同等于数组在背包数组中的下标索引
	 * @param Num - 物品数量
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月08日 星期五 20:07:22
	 */
	UFUNCTION(Client, Reliable, BlueprintCallable, Category="背包组件", DisplayName="客户端根据背包Id更新物品")
	void CC_UpdateItem(const FString& BackpackId, FBackpackItemInfo NewItemInfo);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="背包组件", DisplayName="服务器根据背包Id更新物品")
	void SC_UpdateItem(const FString& BackpackId, FBackpackItemInfo NewItemInfo, float ClientUpdateTime);

	UFUNCTION(BlueprintCallable, Category="背包组件", DisplayName="客户端作为服务器根据背包Id更新物品")
	void SNC_UpdateItem(const FString& BackpackId, FBackpackItemInfo NewItemInfo);
	
	/*
	 * @description: TryRemoveItem - 尝试移除背包物品
	 * @param BackpackId - 物品在背包中的唯一 Id，同等于数组在背包数组中的下标索引
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月08日 星期五 20:07:41
	 */
	UFUNCTION(BlueprintCallable, Category="背包组件", DisplayName="服务器作为客户端尝试移除背包物品")
	void SNC_TryRemoveItem(const FString& BackpackId);

	UFUNCTION(Client, Reliable, BlueprintCallable, Category="背包组件", DisplayName="客户端尝试移除背包物品")
	void CC_TryRemoveItem(const FString& BackpackId);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="背包组件", DisplayName="服务器端尝试移除背包物品")
	void SC_TryRemoveItem(const FString& BackpackId, float RemoveTime);
	/*
	 * @description: CreateItemAfterDiscard - 在丢弃背包物品时，在场景中生成物品 Actor
	 * @param FString - 物品的 Id
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月08日 星期五 20:07:07
	 */
	// TODO - 在游戏过程中，服务器创建的物品尺寸在服务器端正常，而在客户端却缩小了。
	UFUNCTION(Client, Reliable, BlueprintCallable, Category="背包组件", DisplayName="客户端丢弃物品后在场景中生成物品")
	void CC_CreateItemAfterDiscard(const FString& Id);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="背包组件", DisplayName="服务器创建丢弃后在场景中生成的物品")
	void SC_CreateItemAfterDiscard(const FString& Id, FName ItemClientName);

	UFUNCTION(BlueprintCallable, Category="背包组件", DisplayName="服务器作为客户端创建丢弃后在场景中生成的物品")
	void SNC_CreateItemAfterDiscard(const FString& Id);
	/*
	 * @description: GetItemsByType - 根据物品类型获取物品
	 * @param InItemType 物品类型 
	 * @param InItems 物品 
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年08月04日 星期四 11:08:49
	 */
	UFUNCTION(BlueprintCallable, Category="背包组件", DisplayName="根据物品类型获取物品")
	void GetItemsByType(EItemType InItemType, TArray<FBackpackItemInfo>& InItems);

	UFUNCTION(Category="Backpack", DisplayName="服务器背包数据覆盖客户端背包数据")
	void ServerReportBackpackDataOverride(const TArray<FBackpackItemInfo>& ServerBackpackItems);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
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
	 * @description: AddNewItem - 在背包指定位置放置物品
	 * @param Item - 物品
	 * @param Index - 背包位置
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月08日 星期五 20:07:07
	 */
	UFUNCTION(BlueprintCallable, Category="背包", DisplayName="在背包指定位置放置物品")
	void AddNewItem(FBackpackItemInfo Item, int Index);

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
	 */
	UFUNCTION(BlueprintCallable, Category="背包组件", DisplayName="检测指定位置是否可放置物品")
	bool PlaceIndexCheck(FBackpackItemInfo Item, int Index);


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
	 * @description: RemoveItem - 移除指定 BackpackId 的物品
	 * @param BackpackId - 物品在背包中的唯一 Id，同等于数组在背包数组中的下标索引
	 * @return 结果
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月08日 星期五 20:07:22
	 */
	bool RemoveItem(FString BackpackId);

public:

	FORCEINLINE UBackpackWidget* GetBackpackWidget() const { return BackpackWidget;};

	FORCEINLINE TArray<FBackpackItemInfo> GetBackpackItemDatas(){ return Items; };
};

