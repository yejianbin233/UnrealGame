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

	void ShowPickupWidget(bool bIsShowWidget);

	virtual void Fire(const FVector& HitTarget);

	virtual void OnRep_Owner() override;

	void SetHUDAmmo();

	// 当被淘汰时，丢掉武器
	// TODO 当死亡时，不应该直接丢弃武器，而是类似吃鸡掉盒子，武器可以直接扔。
	void Dropped();

	void AddAmmo(int32 AmmoToAdd);

	// 准心由 5 个部分组成
	// 纹理压缩设置"用户界面2D"
	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	class UTexture2D* CrosshairsCenter;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	class UTexture2D* CrosshairsTop;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	class UTexture2D* CrosshairsLeft;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	class UTexture2D* CrosshairsBottom;
	
	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	class UTexture2D* CrosshairsRight;

	/*
	 * 在瞄准时放大视野(FOV)
	 */
	UPROPERTY(EditAnywhere)
	float ZoomedFOV = 30.0f;

	/*
	 * 放大视野的插值过渡速度
	 */
	float ZoomInterpSpeed = 20.0f;

	/*
	 * 自动开火的延迟时间
	 */ 
	UPROPERTY(EditAnywhere, Category="Combat", meta=(AllowPrivateAccess=true))
	float FireDelay=0.15f;

	UPROPERTY(EditAnywhere, Category="Combat", meta=(AllowPrivateAccess=true))
	bool bAutomatic=true;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex
	);

	UPROPERTY()
	class ABlasterCharacter* BlasterOwnerCharacter;
	UPROPERTY()
	class ABlasterPlayerController* BlasterPlayerOwnerController;

private:

	// 武器类具有骨骼网格体
	UPROPERTY(VisibleAnywhere, Category="Weapon Properties")
	class USkeletalMeshComponent* WeaponMesh;

	// 作用：碰撞区域内武器可拾取
	UPROPERTY(VisibleAnywhere, Category="Weapon Properties")
	class USphereComponent* AreaSphere;

	UPROPERTY(ReplicatedUsing="OnRep_WeaponState", VisibleAnywhere, Category="Weapon Properties")
	EWeaponState WeaponState;

	UFUNCTION()
	void OnRep_WeaponState();	

	UPROPERTY(VisibleAnywhere, Category="Weapon Properties")
	class UWidgetComponent* PickupWidget;

	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	class UAnimationAsset* FireAnimtion;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	TSubclassOf<class ACasing> CasingClass;

	// 子弹
	UPROPERTY(EditAnywhere, ReplicatedUsing="OnRep_Ammo")
	int32 Ammo;

	// 当 Ammo 数量更新时，更新 HUD
	UFUNCTION()
	void OnRep_Ammo();

	void SpendRound();

	UPROPERTY(EditAnywhere)
	int32 MagCapacity;

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;
	
public:	
	
	void SetWeaponState(EWeaponState InWeaponState);

	FORCEINLINE USphereComponent* GetAreaSphereComponent(){ return AreaSphere; }

	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const {return WeaponMesh;}

	FORCEINLINE float GetZoomedFOV() const { return ZoomedFOV;}
	
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed;}

	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }

	FORCEINLINE int32 GetAmmo() const { return Ammo; }

	FORCEINLINE int32 GetMagCapacity() const { return MagCapacity;  }

	bool IsEmpty();
};
