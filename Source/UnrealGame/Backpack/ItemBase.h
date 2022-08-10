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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Info", DisplayName="场景物品信息")
	FSceneItemInfo SceneItemInfo;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category="Item Info", DisplayName="物品变换")
	FTransform DeltaTransform;

	UPROPERTY(Replicated, BlueprintReadOnly, Category="Item", DisplayName="实际物品信息")
	FBackpackItemInfo ActualItemInfo;

private:
	// 用于判断物品是否动态生成网格体
	bool bIsInitialized = false;

public:
	// Sets default values for this actor's properties
	AItemBase();

	/*
	 * @description: Init - 用于根据物品信息动态创建物品显示网格体
	 * @param FItemInfo - 物品新
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月09日 星期六 13:07:45
	 */
	UFUNCTION(BlueprintCallable)
	void Init(FItemInfo ItemInfo);

	UFUNCTION(Client, Reliable, Category="Pickup", DisplayName="客户端已拾取处理")
	void CC_PickedUpHandle();
	
	UFUNCTION(NetMulticast, Reliable, Category="Pickup", DisplayName="客户端取消已拾取处理")
    void CC_CanclePickedUpHandle();

	// UFUNCTION(Server, Reliable, Category="Update ItemInfo", DisplayName="更新拾取后的物品信息")
	// void SC_UpdateActualItemInfo(FBackpackItemInfo NewItemInfo);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	/*
	 * @description: GetBackpackItemInfo - 根据场景物品信息获取对应的背包物品信息
	 * 
	 * @return FBackpackItemInfo - 背包物品
	 * ...
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月09日 星期六 13:07:24
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Item Info")
	FBackpackItemInfo GetBackpackItemInfo();
	
private:
	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
