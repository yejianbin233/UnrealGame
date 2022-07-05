// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UnrealGame/HUD/BlasterHUD.h"
#include "UnrealGame/Enums/WeaponTypes.h"
#include "CombatComponent.generated.h"


#define TRACE_LENGTH 80000.0f

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALGAME_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// 声明友类，友类可访问类的所有变量和函数(受保护、私有的)
	friend class ABlasterCharacter;

	void EquipWeapon(class AWeapon* WeaponToEquip);

	void Reload();

	void FireButtonPressed(bool bPressed);
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void SetAiming(bool InbIsAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool InbIsAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();

	UFUNCTION(Server, Reliable)
	void Server_Fire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	void TraceUnderCrosshairs(FHitResult& TraceHitResult);

	void SetHUDCrosshairs(float DeltaTime);

	void InterpFOV(float DeltaTime);

	UFUNCTION(Server, Reliable)
	void ServerReload();

	void HandleReload();

	int32 AmountToReload();
	
private:

	class ABlasterCharacter* BlasterCharacter;

	class ABlasterPlayerController* BlasterPlayerController;

	class ABlasterHUD* BlasterHUD;
	
	UPROPERTY(ReplicatedUsing="OnRep_EquippedWeapon")
	AWeapon* EquippedWeapon;

	UPROPERTY(Replicated)
	bool bIsAiming;

	// 装备武器后的行走速度
	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;
	
	// 装备武器并处于瞄准状态下的行走速度
	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;

	bool bFireButtonPressed;

	// 速度
	float CrosshairVelocityFactor;
	// 空中
	float CrosshairInAirFactor;

	// 瞄准
	float CrosshairAimFactor;

	// 射击
	float CrosshairShootingFactor;

	FVector HitTarget;

	FCrosshairHUDPackage HUDPackage;

	/*
	 * 瞄准和视野
	 */
	UPROPERTY(EditAnywhere, Category="Camera", meta=(AllowPrivateAccess=true))
	float DefaultFOV; // 默认的摄像机视野

	UPROPERTY(EditAnywhere, Category="Camera", meta=(AllowPrivateAccess=true))
	float ZoomedFOV=30.0f;

	UPROPERTY(EditAnywhere, Category="Camera", meta=(AllowPrivateAccess=true))
	float ZoomInterpSpeed=20.0f;

	UPROPERTY(EditAnywhere, Category="Camera", meta=(AllowPrivateAccess=true))
	float CurrentFOV;

	/*
	 * 自动开火功能能
	 */
	FTimerHandle FireTimer;
	
	bool bCanFire;
	
	void StartFireTimer();
	void FireTimerFinished();

	void Fire();

	bool CanFire();
	// 自动开火

	// 当前武器的弹药
	UPROPERTY(ReplicatedUsing = "OnRep_CarriedAmmo")
	int32 CarriedAmmo;

	UFUNCTION()
	void OnRep_CarriedAmmo();

	// 拥有的弹药
	TMap<EWeaponType, int32> CarriedAmmoMap;

	void InitializeCarriedAmmo();

	UPROPERTY(ReplicatedUsing="OnRep_CombatState")
	ECombatState CombatState=ECombatState::ECS_Unoccupied;

	UFUNCTION()
	void OnRep_CombatState();

	void UpdateAmmoValues();
};
