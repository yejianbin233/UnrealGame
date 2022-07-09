// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UnrealGame/Enums/WeaponTypes.h"
#include "Weapon.generated.h"


// 定义武器的枚举类型
UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName="Initial State"),
	EWS_Equipped UMETA(DisplayName="Equipped"),
	EWS_Dropped UMETA(DisplayName="Dropped"),

	EWS_MAX UMETA(DisplayName="DefaultMax")
};


// TODO 改为枪武器类型
UCLASS()
class UNREALGAME_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// TODO 当死亡时，不应该直接丢弃武器，而是类似吃鸡掉盒子，武器可以直接扔。
	// 当被淘汰时，丢掉武器
	void Dropped();

	// void SetHUDAmmo();

	// void AddAmmo(int32 AmmoToAdd);

	// void ShowPickupWidget(bool bIsShowWidget);

	/* 射击功能 */
	virtual void Fire(const FVector& HitTarget);

	virtual void AutoFire();
	/* 射击功能 */

public:

	UPROPERTY(EditDefaultsOnly, Category="Component")
	TSubclassOf<class UCollimationComponent> CollimationComponentClass;
	
	// UPROPERTY(VisibleAnywhere, Category="Component")
	// class UPickableComponent* PickableComponent;

	/*
	 * 在瞄准时放大视野(FOV)
	 */
	UPROPERTY(EditAnywhere, Category="Aim Properties")
	float ZoomedFOV = 30.0f;

	/*
	 * 放大视野的插值过渡速度
	 */
	UPROPERTY(EditAnywhere, Category="Aim Properties")
	float ZoomInterpSpeed = 20.0f;

	/*
	 * 射击开火的延迟时间
	 */ 
	UPROPERTY(EditAnywhere, Category="Shot Properties", meta=(AllowPrivateAccess=true))
	float FireDelay = 0.15f;

	/*
	 * 是否允许自动射击
	 */
	UPROPERTY(EditAnywhere, Category="Shot Properties", meta=(AllowPrivateAccess=true))
	bool bAutomatic = true;
	
protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	/* 网络复制 */

	virtual void OnRep_Owner() override;

	// 当 Ammo 数量更新时，更新 HUD
	// UFUNCTION()
	// void OnRep_Ammo();

	UFUNCTION()
	void OnRep_WeaponState();

	/* 网络复制 */

	// void SpendRound();

	
private:

	class ABlasterCharacter* BlasterOwnerCharacter;
	
	class ABlasterPlayerController* BlasterPlayerOwnerController;
	// 枪骨骼网格体
	UPROPERTY(VisibleAnywhere, Category="Gun Properties")
	class USkeletalMeshComponent* WeaponMesh;

	// 武器状态
	UPROPERTY(ReplicatedUsing="OnRep_WeaponState", VisibleAnywhere, Category="Gun Properties")
	EWeaponState WeaponState;

	// 射击动画
	UPROPERTY(EditAnywhere, Category="Gun Properties")
	class UAnimationAsset* FireAnimtion;

	// 射击消耗完毕后的子弹弹射
	UPROPERTY(EditAnywhere, Category = "Ammo Properties")
	TSubclassOf<class ACasing> CasingClass;

	// 装弹数量
	UPROPERTY(Replicated, EditAnywhere, Category = "Ammo Properties")
	int32 Ammo;

	// 最大装弹数量
	UPROPERTY(EditAnywhere, Category = "Ammo Properties")
	int32 MaxReloadAmmoAmount;

	// 武器类型
	UPROPERTY(EditAnywhere, Category="Gun Properties")
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, Category="Pickable Properties")
	TEnumAsByte<ECollisionChannel> PickableCollisionChannel;
	
public:	
	
	void SetWeaponState(EWeaponState InWeaponState);

	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const {return WeaponMesh;}

	FORCEINLINE float GetZoomedFOV() const { return ZoomedFOV;}
	
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed;}

	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }

	FORCEINLINE int32 GetAmmo() const { return Ammo; }

	FORCEINLINE int32 GetMaxReloadAmmoAmount() const { return MaxReloadAmmoAmount;  }

	bool IsEmpty();
};
