// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponAsset.h"
#include "LongRangeWeaponAsset.generated.h"

/**
 * 
 */
UCLASS()
class UNREALGAME_API ULongRangeWeaponAsset : public UWeaponAsset
{
	GENERATED_BODY()

	ULongRangeWeaponAsset();

public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Fire", DisplayName="武器开火动画蒙太奇")
	UAnimMontage* WeaponFireAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Fire", DisplayName="角色开火动画蒙太奇")
	UAnimMontage* CharacterFireAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ammo", DisplayName="武器填充子弹动画蒙太奇")
	UAnimMontage* WeaponReloadAnimMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ammo", DisplayName="角色填充子弹动画蒙太奇")
	UAnimMontage* CharacterReloadAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ammo", DisplayName="子弹填装剩余子弹耗尽动画蒙太奇")
	UAnimMontage* WeaponAmmoExhaustAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Fire", DisplayName="子弹发射插槽名称")
	FName FireProjectileSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Fire", DisplayName="子弹壳发射插槽名称")
	FName FireCasingSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Fire", DisplayName="连续开火间隔(秒)")
	float FireInterval;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Fire", DisplayName="连续开火过热曲线")
	UCurveFloat* FireOverHeatCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ammo", DisplayName="子弹最大填充量")
	int32 MaxReloadAmmoAmount;
};
