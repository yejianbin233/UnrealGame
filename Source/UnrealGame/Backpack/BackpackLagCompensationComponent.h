// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemBase.h"
#include "ItemInfoObject.h"
#include "Components/ActorComponent.h"
#include "UnrealGame/Struct/UnrealGameStruct.h"
#include "BackpackLagCompensationComponent.generated.h"

// USTRUCT(BlueprintType)
// struct FBackpackItemChangedData
// {
// 	GENERATED_BODY()
//
// public:
// 	float ChagedTime;
// };

DECLARE_EVENT_ThreeParams(UBackpackLagCompensationComponent, FOnServerReportBackpackDataOverride, AItemBase* Item, FBackpackItemInfo BackpackItemInfo, int Index)
// DECLARE_EVENT_OneParam(UBackpackLagCompensationComponent, FOnServerReportBackpackDataOverride, const TArray<UItemInfoObject*>&)
DECLARE_EVENT_OneParam(UBackpackLagCompensationComponent, FOnClientBackpackItemChanged, float);
DECLARE_EVENT_OneParam(UBackpackLagCompensationComponent, FOnServerReportBackpackItemChanged, float);

/*
 * 客户端维护的背包拾取滞后处理
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALGAME_API UBackpackLagCompensationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category="Reference", DisplayName="玩家角色")
	class ABlasterCharacter* PlayerCharacter;

	FOnServerReportBackpackDataOverride OnServerReportBackpackDataOverride;
	
private:
	TDoubleLinkedList<float> BackpacItemChangedDates;

public:	
	// Sets default values for this component's properties
	UBackpackLagCompensationComponent();

	/* note：服务器作为客户端时不需要 UBackpackLagCompensationComponent 的功能
	 */
	
	/*
	 *	ServerFeedbackPickupItemFailture - 在客户端拾取场景物品时，如果本地模拟成功就会隐藏物品，当在服务器拾取失败时，会将客户端物品的隐藏状态变为显示状态
	 */
	UFUNCTION(Client, Reliable, Category="Backpack", DisplayName="服务器反馈背包拾取物品结果")
	void ServerFeedbackPickupItemFailture(AItemBase* PickupFailtureItem);

	/*
	 *	CacheBackpackItemChangedData - 客户端在修改背包物品数据时(添加、使用)广播调用 - 记录背包的修改时间
	 */
	UFUNCTION(Client, Reliable, Category="Backpack", DisplayName="缓存背包物品变更数据")
	void CacheBackpackItemChangedData(float BackpackItemChangedTime);

	/*
	 * ServerFeedbackBackpackItemChangedResult - 客户端每次修改背包数据时都会请求对应的服务器函数处理数据。
	 *
	 * 客户端 -> 服务器会传输当次的修改时间，当服务器处理完毕后会调用 ServerFeedbackBackpackItemChangedResult 根据时间判断客户端的背包数据是否需要更新(使用服务器数据覆盖)
	 */
	UFUNCTION(Client, Reliable, Category="Backpack", DisplayName="服务器反馈背包物品变更结果")
	void ServerFeedbackBackpackItemChangedResult(float BackpackItemChangedTime);

	/*
	 * ClientRequestServerBackpackDataOverride - 优化，在需要更新时向服务器请求更新，而不用在 ServerFeedbackBackpackItemChangedResult 每次传递服务器背包数据
	 */
	UFUNCTION(Server, Reliable, Category="Backpack", DisplayName="客户端请求服务器背包数据覆盖")
	void ClientRequestServerBackpackDataOverride();

	/*
	 * 当需要覆盖客户端背包数据时，从服务器反馈的背包数据
	 */
	UFUNCTION(Client, Reliable, Category="Backpack", DisplayName="服务器回报服务器背包物品数据")
	void ServerReportBackpackData(AItemBase* Item, FBackpackItemInfo BackpackItemInfo, int Index);
	// void ServerReportBackpackData(const TArray<UItemInfoObject*>& ServerBackpackItems);
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	
};
