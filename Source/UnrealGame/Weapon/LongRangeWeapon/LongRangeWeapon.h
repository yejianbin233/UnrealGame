// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Weapon.h"
#include "UnrealGame/Component/LagCompensation/LRWLagCompensationComponent.h"
#include "UnrealGame/Struct/UnrealGameStruct.h"
#include "LongRangeWeapon.generated.h"


/**
 * 
 */
UCLASS()
class UNREALGAME_API ALongRangeWeapon : public AWeapon
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditDefaultsOnly, Category="Collimation", DisplayName="准星组件类")
	TSubclassOf<class UCollimationComponent> CollimationComponentClass;

	UPROPERTY(BlueprintReadOnly, Category="Collimation", DisplayName="准星组件")
	UCollimationComponent* CollimationComponent;

	UPROPERTY(EditDefaultsOnly, Category="Aim", DisplayName="瞄准组件类")
	TSubclassOf<class UAimComponent> AimComponentClass;

	UPROPERTY(BlueprintReadOnly, Category="Aim", DisplayName="瞄准组件")
	UAimComponent* AimComponent;

	UPROPERTY(BlueprintReadOnly, Category="Lag Compensation", DisplayName="远程武器滞后补偿组件")
	ULRWLagCompensationComponent* LagCompensationComponent;
	
	// 发射子弹效果
	UPROPERTY(Replicated, BlueprintReadWrite, Category="Ammo", DisplayName="子弹数据")
	FProjectileData ProjectileData;

	UPROPERTY(BlueprintReadWrite, Category="Fire", DisplayName="武器开火动画蒙太奇")
	UAnimMontage* WeaponFireAnimMontage;

	UPROPERTY(BlueprintReadWrite, Category="Fire", DisplayName="角色开火动画蒙太奇")
	UAnimMontage* CharacterFireAnimMontage;

	UPROPERTY(BlueprintReadWrite, Category="Fire", DisplayName="武器填充子弹动画蒙太奇")
	UAnimMontage* WeaponReloadAnimMontage;

	UPROPERTY(BlueprintReadWrite, Category="Fire", DisplayName="角色填充子弹动画蒙太奇")
	UAnimMontage* CharacterReloadAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ammo", DisplayName="子弹填装剩余子弹耗尽动画蒙太奇")
	UAnimMontage* WeaponAmmoExhaustAnimMontage;

	UPROPERTY(BlueprintReadOnly, Category="Fire", DisplayName="子弹发射插槽名称")
	FName FireProjectileSocketName;

	UPROPERTY(BlueprintReadOnly, Category="Fire", DisplayName="子弹发射插槽名称")
	FName FireCasingSocketName;
	
	UPROPERTY(BlueprintReadWrite, Category="Fire", DisplayName="射击间隔")
	float FireInterval;

	UPROPERTY(BlueprintReadOnly, Category="Fire", DisplayName="连续开火定时器")
	FTimerHandle FireHoldTimerHandle;
	
	UPROPERTY(BlueprintReadWrite, Category="Ammo", DisplayName="装填子弹剩余数量")
	int32 LoadAmmo;

	// 最大装弹数量
	UPROPERTY(BlueprintReadWrite, Category="Ammo", DisplayName="子弹最大填充量")
	int32 MaxReloadAmmoAmount;

	UPROPERTY(BlueprintReadWrite, Category="Ammo", DisplayName="子弹数据表")
	UDataTable* ProjectileDataTable;

	// 当填装的子弹改变时
	FOnLoadAmmoChanged OnLoadAmmoChanged;
	// 当服务器反馈子弹数量改变时
	FOnServerLoadAmmoChangedFeedback OnServerLoadAmmoChangedFeedback;

private:
	UPROPERTY(BlueprintReadWrite, Category="Fire", DisplayName="当前射击间隔", meta=(AllowPrivateAccess))
	float CurrentFireInterval;

	UPROPERTY(BlueprintReadWrite, Category="Fire", DisplayName="是否正在播放开火动画", meta=(AllowPrivateAccess))
	bool bIsPlayCharacterFireAnimMontage;

	UPROPERTY(BlueprintReadWrite, Category="Reload", DisplayName="是否正在播放填装子弹动画", meta=(AllowPrivateAccess))
	bool bIsPlayWeaponReloadAnimMontage;

	UPROPERTY(BlueprintReadWrite, Category="Ammo", DisplayName="是否正在播放缺少子弹动画", meta=(AllowPrivateAccess))
	bool bIsPlayWeaponAmmoExhaustAnimMontage;
	
public:

	ALongRangeWeapon();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void InitHandle() override;

	virtual void Use(ABlasterCharacter* InPlayerCharacter) override;

	virtual void Equip() override;

	virtual void SetEquptCharacter(ABlasterCharacter* BlasterCharacter) override;

	// virtual void SNC_Equip() override;
	//
	virtual void SC_Equip() override;
	//
	virtual void CC_Equip() override;
	//
	virtual void NM_Equip() override;
	//
	virtual void NM_EquipExceptClient() override;

	virtual void UnEquip() override;

	// virtual void SNC_UnEquip() override;
	//
	// virtual void SC_UnEquip() override;
	//
	// virtual void CC_UnEquip() override;
	//
	// virtual void NM_UnEquip() override;
	//
	// virtual void NM_UnEquip_Implementation() override;
	//
	// virtual void UnEquipHandle() override;

	UFUNCTION(Client, Reliable, Category="Equipment", DisplayName="装备客户端处理")
	void CC_EquipmentHandle(bool Equipped);

	UFUNCTION(NetMulticast, Reliable, Category="Equipment", DisplayName="装备多播处理")
	void NM_EquipmentHandle(bool Equipped);
	
	/* 射击功能 */
	UFUNCTION(Client, Reliable, BlueprintCallable, Category="Fire", DisplayName="开火")
	void CC_Fire();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Fire", DisplayName="开火")
	void SC_Fire(float ClientFireTime);

	UFUNCTION(BlueprintCallable, Category="Fire", DisplayName="开火")
	void SNC_Fire();

	UFUNCTION(Client, Reliable, Category="Fire", DisplayName="连续开火")
	void CC_FireHold();

	UFUNCTION(BlueprintCallable, Category="Fire", DisplayName="服务器作为客户端连续开火")
	void SNC_FireHold();
	
	UFUNCTION(Client, Reliable, Category="Fire", DisplayName="连续开火")
	void CC_FireHoldHandle();

	UFUNCTION(BlueprintCallable, Category="Fire", DisplayName="服务器作为客户端连续开火")
	void SNC_FireHoldHandle();

	UFUNCTION(Client, Reliable, Category="Fire", DisplayName="客户端暂停连续开火")
	void CC_FireHoldStopHandle();

	UFUNCTION(BlueprintCallable, Category="Fire", DisplayName="服务器作为客户端展厅连续开火")
	void SNC_FireHoldStopHandle();
	/* 射击功能 */

	/*填装子弹*/
	UFUNCTION(Client, Reliable, BlueprintCallable, Category="Ammo", DisplayName="客户端装填")
	void CC_Reload();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Ammo", DisplayName="服务器装填")
	void SC_Reload(float ClientReloadTime);

	UFUNCTION(BlueprintCallable, Category="Ammo", DisplayName="服务器作为客户端装填")
	void SNC_Reload();

	UFUNCTION(Client, Reliable, DisplayName="客户端设置子弹类型")
	void CC_SetProjectileData(FName ProjectileId);
	
	UFUNCTION(NetMulticast, Reliable, DisplayName="服务器设置子弹类型")
	void SC_SetProjectileData(FName ProjectileId);

	void SetProjectileData(FName ProjectileId);

	// 生成子弹函数
	UFUNCTION(Client, Reliable, BlueprintCallable, Category="Ammo", DisplayName="客户端生成子弹")
	void CC_SpawnProjectile();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Ammo", DisplayName="服务器生成子弹")
	void SC_SpawnProjectile();

	UFUNCTION(BlueprintCallable, Category="Ammo", DisplayName="服务器作为客户端生成子弹")
	void SNC_SpawnProjectile();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category="Ammo", DisplayName="多播生成子弹")
	void NM_SpawnProjectile();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category="Ammo", DisplayName="多播(除了客户端)生成子弹")
	void NM_SpawnProjectileExceptClient();
	// 生成子弹函数

	// 生成子弹壳函数
	UFUNCTION(BlueprintCallable, Category="Ammo", DisplayName="生成子弹")
	void SpawnProjectile();
	
	UFUNCTION(BlueprintCallable, Category="Ammo", DisplayName="生成子弹壳")
	void SpawnCasing();
	/*填装子弹*/
	
	// note 如果 PlayCharacterMontage 和 PlayWeaponMontage 一起调用，应先调用 PlayCharacterMontage
	UFUNCTION(BlueprintCallable, Category="Ammo", DisplayName="播放玩家角色蒙太奇动画")
	void PlayCharacterMontage(UAnimMontage* AnimMontage);

	UFUNCTION(BlueprintCallable, Category="Ammo", DisplayName="播放武器蒙太奇动画")
	void PlayWeaponMontage(UAnimMontage* AnimMontage);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category="Ammo", DisplayName="除了客户端都播放玩家角色蒙太奇动画")
	void PlayCharacterMontageExceptClient(UAnimMontage* AnimMontage);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category="Ammo", DisplayName="除了客户端都播放武器蒙太奇动画")
	void PlayWeaponMontageExceptClient(UAnimMontage* AnimMontage);

protected:

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

private:

	void PlayMontageStarted(UAnimMontage* AnimMontage);
	
	void PlayMontageEnded(UAnimMontage* AnimMontage, bool bInterrupted);
	
public:
	
	FORCEINLINE int32 GetAmmo() const { return LoadAmmo; }

	FORCEINLINE int32 GetMaxReloadAmmoAmount() const { return MaxReloadAmmoAmount;  }

	bool IsAmmoEmpty();
};





