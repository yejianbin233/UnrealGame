// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemInfoObject.h"
#include "GameFramework/Actor.h"
#include "ItemBase.generated.h"

UCLASS()
class UNREALGAME_API AItemBase : public AActor
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item", DisplayName="物理模拟静态网格体组件")
	class UStaticMeshComponent* PhysicMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Pickup",DisplayName="可拾取区域组件")
	class USphereComponent* PickableAreaComponent;

private:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="物品数据表唯一 Id", meta=(AllowPrivateAccess))
	FName Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Info", DisplayName="物品变换", meta=(AllowPrivateAccess))
	// UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category="Item Info", DisplayName="物品变换")
	FTransform DeltaTransform;

	// UPROPERTY(Replicated, BlueprintReadOnly, Category="Item", DisplayName="实际物品信息", meta=(AllowPrivateAccess))
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category="Item", DisplayName="实际物品信息", meta=(AllowPrivateAccess))
	FBackpackItemInfo ActualItemInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Item Data Table", DisplayName="物品数据表", meta=(AllowPrivateAccess))
	UDataTable* DT_Item;

	UPROPERTY(BlueprintReadWrite, Category="Item Data", DisplayName="物品数据表数据", meta=(AllowPrivateAccess))
	FItemInfo DT_ItemInfo;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Item Use", DisplayName="物品使用类", meta=(AllowPrivateAccess))
	TSubclassOf<AItemUse> ItemUseClass;

	// // 场景中的可拾取物品可以根据功能，选择是否初始化 ItemUse
	// /*
	//  * 设想场景，如：可交互蜡烛
	//  * 1. 蜡烛可以拾取 - AItemBase
	//  * 2. 可以交互(点亮) - IInteractiveInterface
	//  * 3. 可以装备 - ItemUse
	//  *
	//  * 其中 AItemBase 和 ItemUse 都是 Actor，但场景中只需要显示其中一个，可以将其所属的 Mesh 隐藏起来或暂时设置为无网格体
	//  */
	// UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Item Use", DisplayName="物品使用Actor", meta=(AllowPrivateAccess))
	// AItemUse* ItemUse;
	UPROPERTY(Replicated, BlueprintReadWrite, Category="BackpackItem", DisplayName="背包物品", meta=(AllowPrivateAccess))
	UItemInfoObject* ItemInfoObject;

private:
	
	bool bIsInitialized = false;

public:
	// Sets default values for this actor's properties
	AItemBase();

	virtual bool ReplicateSubobjects (class UActorChannel *Channel, class FOutBunch *Bunch, FReplicationFlags *RepFlags) override;

	/*
	 * @description: Init - 用于根据物品信息动态创建物品显示网格体
	 * @param FItemInfo - 物品新
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月09日 星期六 13:07:45
	 */
	UFUNCTION(BlueprintCallable, Category="Item Init", DisplayName="初始化")
	void Init();

	UFUNCTION(BlueprintCallable, Category="Item", DisplayName="将物品放置到场景中")
	void PutInSceneHandle(FVector PutLocation);

	UFUNCTION(BlueprintCallable, Category="Item", DisplayName="将物品放置到背包组件中")
	void PutInBackpackHandle();
	
	UFUNCTION(Category="Pickup", DisplayName="隐藏场景物品")
	void Hide();

	UFUNCTION(Category="Pickup", DisplayName="服务器作为客户端隐藏场景物品")
	void SNC_Hide();

	UFUNCTION(Server, Reliable, Category="Pickup", DisplayName="服务器隐藏场景物品")
	void SC_Hide();

	UFUNCTION(Client, Reliable, Category="Pickup", DisplayName="客户端隐藏场景物品")
	void CC_Hide();
	
	UFUNCTION( Category="Pickup", DisplayName="显示场景物品")
	void Show();

	UFUNCTION(Category="Pickup", DisplayName="服务器作为客户端显示场景物品")
	void SNC_Show();

	UFUNCTION(Server, Reliable, Category="Pickup", DisplayName="服务器显示场景物品")
	void SC_Show();

	UFUNCTION(Client, Reliable, Category="Pickup", DisplayName="客户端显示场景物品")
	void CC_Show();

	UFUNCTION(Client, Reliable, Category="Put", DisplayName="客户端放置场景物品")
	void CC_PutTargetLocation(FVector TargetLocation);

	UFUNCTION(Server, Reliable, Category="Put", DisplayName="服务器放置场景物品")
	void SC_PutTargetLocation(FVector TargetLocation);

	UFUNCTION(NetMulticast, Reliable, Category="Put", DisplayName="服务器放置场景物品")
	void NM_PutTargetLocation(FVector TargetLocation);

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	/*
	 * @description: GetItemInfo - 根据场景物品信息获取对应的背包物品信息
	 * 
	 * @return FBackpackItemInfo - 背包物品
	 * ...
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月09日 星期六 13:07:24
	 */
	UFUNCTION(BlueprintCallable, Category="Item Info", DisplayName="获取物品信息")
	FORCEINLINE FBackpackItemInfo GetItemInfo() { return ActualItemInfo; }

	FORCEINLINE FName GetItemId() const { return Id;}

	FORCEINLINE TSubclassOf<AItemUse> GetItemUseClass() const { return ItemUseClass; }
	
	FORCEINLINE UItemInfoObject* GetItemInfoObject() const { return ItemInfoObject; }
	
	FORCEINLINE FItemInfo GetDTItemInfo() const { return DT_ItemInfo; }
};
