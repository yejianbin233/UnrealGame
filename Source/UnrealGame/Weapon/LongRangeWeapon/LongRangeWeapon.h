// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Weapon.h"
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

	// 发射子弹效果
	UPROPERTY(BlueprintReadWrite, Category="Ammo", DisplayName="子弹数据")
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

	UPROPERTY(BlueprintReadOnly, Category="Fire", DisplayName="连续开火过热曲线")
	UCurveFloat* FireOverHeatCurve;

	UPROPERTY(BlueprintReadOnly, Category="Fire", DisplayName="连续开火定时器")
	FTimerHandle FireHoldTimerHandle;
	
	// 装弹数量 TODO 
	// UPROPERTY(ReplicatedUsing="", BlueprintReadWrite, Category="Ammo", DisplayName="装填子弹剩余数量")
	UPROPERTY(BlueprintReadWrite, Category="Ammo", DisplayName="装填子弹剩余数量")
	int32 LoadAmmo;

	// 最大装弹数量
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ammo", DisplayName="子弹最大填充量")
	int32 MaxReloadAmmoAmount;

private:
	UPROPERTY(BlueprintReadWrite, Category="Fire", DisplayName="当前射击间隔", meta=(AllowPrivateAccess))
	float CurrentFireInterval;

public:

	ALongRangeWeapon();

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void Init(ABlasterCharacter* InPlayerCharacter, ABlasterPlayerController* InPlayerController) override;

	virtual void Equipment(bool Equipped) override;
	
	/* 射击功能 */
	UFUNCTION(BlueprintCallable, Category="Fire", DisplayName="开火")
	virtual void Fire();

	UFUNCTION(BlueprintCallable, Category="Fire", DisplayName="连续开火")
	virtual void FireHold();

	virtual void FireButtonReleased();
	/* 射击功能 */

	UFUNCTION(BlueprintCallable, Category="Ammo", DisplayName="装填")
	virtual void Reload();

protected:
	virtual void SpawnAmmoProjectile();
	virtual void SpawnCasing();

public:
	
	FORCEINLINE int32 GetAmmo() const { return LoadAmmo; }

	FORCEINLINE int32 GetMaxReloadAmmoAmount() const { return MaxReloadAmmoAmount;  }

	bool IsAmmoEmpty();
};

inline void ALongRangeWeapon::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(FireHoldTimerHandle, this, &ALongRangeWeapon::Fire, FireInterval);
	GetWorld()->GetTimerManager().PauseTimer(FireHoldTimerHandle);
}
