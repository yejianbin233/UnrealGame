// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "GunWeapon.generated.h"

/**
 * 
 */
UCLASS()
class UNREALGAME_API AGunWeapon : public AWeapon
{
	GENERATED_BODY()

public:
	AGunWeapon();
	
	UPROPERTY(EditDefaultsOnly, Category="Aim", DisplayName="瞄准组件类")
	TSubclassOf<class UCollimationComponent> CollimationComponentClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Aim", DisplayName="瞄准时视野值(FOV)")
	float ZoomedFOV = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Aim", DisplayName="瞄准时视野过渡速度")
	float ZoomInterpSpeed = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fire", DisplayName="射击间隔")
	float FireDelay = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fire", DisplayName="是否允许自动射击")
	bool bAutomatic = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fire", DisplayName="射击动画")
	class UAnimationAsset* FireAnimtion;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fire", DisplayName="子弹壳类")
	TSubclassOf<class ACasing> CasingClass;

	// 装弹数量
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ammo", DisplayName="弹夹子弹数量")
	int32 LoadAmmo;

	// 最大装弹数量
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ammo", DisplayName="子弹最大填充量")
	int32 MaxReloadAmmoAmount;
	
public:

	virtual void Init() override;
	
	/* 射击功能 */
	virtual void Fire(const FVector& HitTarget);

	virtual void AutoFire();
	/* 射击功能 */

public:
	FORCEINLINE float GetZoomedFOV() const { return ZoomedFOV;}
	
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed;}

	FORCEINLINE int32 GetAmmo() const { return LoadAmmo; }

	FORCEINLINE int32 GetMaxReloadAmmoAmount() const { return MaxReloadAmmoAmount;  }

	bool IsAmmoEmpty();
};
