// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UnrealGame/Actor/ItemUse/ItemUse.h"
#include "UnrealGame/Backpack/ItemInfoObject.h"
#include "UnrealGame/Enums/UnrealGameEnumInterface.h"
#include "UnrealGame/HUD/BlasterHUD.h"
#include "CombatComponent.generated.h"


#define TRACE_LENGTH 80000.0f


DECLARE_EVENT_OneParam(UCombatComponent, FOnBackpackThrowItem, UItemInfoObject* /*丢弃物品*/)

/*
 * TODO - 是否需要设计战斗组件 - 根据装备使用的武器类型进行切换，不同战斗组件提供不同功能；还是各自的装备武器(Weapon)分别定义?
 *
 * 定义可使用三种类型的武器：
 * 1. 远程武器 - 枪、弓箭等
 * 2. 近战武器 - 刀、剑等
 * 3. 可投掷武器 - 手雷等
 *
 * 武器切换控件
 * 通过 1~3 分别切换类型武器
 *
 * 与背包的联动
 * 通过鼠标中键滚动分别切换同类型武器中的其他武器
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALGAME_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()
	
	friend class ABlasterCharacter;
	
public:

	UPROPERTY(Replicated, BlueprintReadWrite, Category="Reference", DisplayName="玩家角色")
	class ABlasterCharacter* PlayerCharacter;

	UPROPERTY(ReplicatedUsing = "OnRep_EquippedWeapon", BlueprintReadWrite, Category="Weapon", DisplayName="当前装备使用中的武器")
	class AWeapon* EquippedWeapon;
	
	UPROPERTY(Replicated, BlueprintReadWrite, Category="Weapon", DisplayName="远程武器")
	class ALongRangeWeapon* LongRangeWeapon;

	UPROPERTY(Replicated, BlueprintReadWrite, Category="Weapon", DisplayName="近战武器")
	class AMeleeWeapon* MeleeWeapon;

	UPROPERTY(Replicated, BlueprintReadWrite, Category="Weapon", DisplayName="可投掷武器")
	class AThrowableWeapon* ThrewableWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat Asset", DisplayName="资产类型数组")
	TArray<FPrimaryAssetType> LoadAssetType;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Aim", DisplayName="正在瞄准中")
	bool bIsAiming;

	// 背包丢弃物品事件
	FOnBackpackThrowItem OnBackpackThrowItem;

public:	
	// Sets default values for this component's properties
	UCombatComponent();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// 声明友类，友类可访问类的所有变量和函数(受保护、私有的)

	// note：不设计客户端可拾取物品
	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Weapon", DisplayName="服务器装备")
	void SC_Equipment(class AItemBase* EquipItem, float EquipTime);

	UFUNCTION(BlueprintCallable, Category="Weapon", DisplayName="服务器作为客户端装备")
	void SNC_Equipment(AItemUse* Item);

	UFUNCTION(Client, Reliable, BlueprintCallable, Category="Weapon", DisplayName="服务器取消装备")
	void CC_UnEquipment();
	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Weapon", DisplayName="服务器取消装备")
	void SC_UnEquipment();
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category="Weapon", DisplayName="多播(除了客户端)取消装备")
	void NM_UnEquipmentExceptClient();
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category="Weapon", DisplayName="多播取消装备")
	void NM_UnEquipment();
	UFUNCTION(BlueprintCallable, Category="Weapon", DisplayName="服务器作为客户端取消装备")
	void SNC_UnEquipment();
	UFUNCTION(BlueprintCallable, Category="Weapon", DisplayName="取消装备")
	void UnEquipment();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Weapon Aim", DisplayName="服务器瞄准")
	void SC_Aim(bool bToAim);
	
	UFUNCTION(Client, Reliable, BlueprintCallable, Category="Weapon Aim", DisplayName="客户端瞄准")
	void C_Aim(bool bToAim);

	UFUNCTION(BlueprintCallable, Category="Weapon Aim", DisplayName="客户端瞄准")
	void Aim(bool bToAim);

	UFUNCTION(Client, Reliable, BlueprintCallable, Category="Weapon Aim", DisplayName="客户端开火")
	void CC_Fire();

	UFUNCTION(BlueprintCallable, Category="Weapon Aim", DisplayName="服务器作为客户端开火")
	void SNC_Fire();

	UFUNCTION(Client, Reliable, BlueprintCallable, Category="Weapon Aim", DisplayName="客户端连续开火")
	void CC_FireHold();

	UFUNCTION(BlueprintCallable, Category="Weapon Aim", DisplayName="服务器作为客户端连续开火")
	void SNC_FireHold();

	UFUNCTION(Client, Reliable, BlueprintCallable, Category="Weapon Aim", DisplayName="客户端连续开火停止")
	void CC_FireHoldStop();
	
	UFUNCTION(BlueprintCallable, Category="Weapon Aim", DisplayName="服务器作为客户端连续开火停止")
	void SNC_FireHoldStop();

	UFUNCTION(Client, Reliable, BlueprintCallable, Category="Weapon Aim", DisplayName="客户端装填子弹")
	void CC_Reload();

	UFUNCTION(BlueprintCallable, Category="Weapon Aim", DisplayName="服务器作为客户端装填子弹")
	void SNC_Reload();
	
	UFUNCTION(BlueprintCallable, Category="Weapon", DisplayName="获取玩家装备状态")
	EPlayerEquipState GetPlayerEquipState();

	virtual void BackpackThrowItemHandle(UItemInfoObject* ThrowItemObject);
	
protected:

	
private:

	UFUNCTION()
	void OnRep_EquippedWeapon();

	void LoadAsset();
	
private:

};
