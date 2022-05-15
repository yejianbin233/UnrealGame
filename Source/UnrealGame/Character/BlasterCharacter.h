// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "UnrealGame/Enums/UnrealGameEnumInterface.h"
#include "UnrealGame/Enums/WeaponTypes.h"
#include "UnrealGame/Interfaces/InteractWithCrosshairsInterface.h"
#include "BlasterCharacter.generated.h"

UCLASS()
class UNREALGAME_API ABlasterCharacter : public ACharacter, public IInteractWithCrosshairsInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABlasterCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PostInitializeComponents() override;

	// 播放开火动画蒙太奇
	void PlayFireMontage(bool bIsAiming);

	// 播放受击动画蒙太奇
	void PlayHitReactMontage();

	// 播放重装弹蒙太奇
	void PlayReloadMontage();

	void Elim();

	UFUNCTION(NetMulticast, Reliable)
	void PlayElimMontage();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/*
	 * 玩家输入处理函数
	 */
	void MoveForward(float Value);

	void MoveRight(float Value);

	void Turn(float Value);

	void LookUp(float Value);

	void JumpButtonPressed();
	
	void EquipButtonPressed();

	void CrouchButtonPressed();

	void AimButtonPressed();
	void AimButtonReleased();

	void FireButtonPressed();
	void FireButtonReleased();

	void ReloadButtonPressed();
	// 玩家输入处理函数

	// 动画蓝图优化函数
	void AimOffset(float DeltaTime);

	void TurnInPlace(float DeltaTime);

	void SimProxiesTurn();

	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor
		, float Damage
		, const UDamageType* DamageType
		, class AController* InstigatorController
		, AActor* DamagerCauser);

	void UpdateHUDHealth();

	
	void PollInit();
	
private:

	/*
	 * 组件
	 */
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	class UWidgetComponent* OverheadWidget;

	UPROPERTY(ReplicatedUsing="OnRep_OverlappingWeapon")
	class AWeapon* OverlappingWeapon;

	//

	/*
	 * 函数
	 */ 
	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);
	
	UFUNCTION(Server, Reliable)
	void ServerEquipButtomPressed();

	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* Combat;

	// 当摄像机接近网格体时使其隐藏
	void HideCameraIfCharacterClose();

	UFUNCTION()
	void OnRep_Health();

	

	/**
	 * 成员属性
	 */
	float AO_Yaw;
	float AO_Pitch;
	float InterpAO_Yaw;

	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;

	UPROPERTY(EditAnywhere, Category="Combat")
	class UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category="Combat")
	class UAnimMontage* ReloadMontage;
	// 被击打动画蒙太奇
	UPROPERTY(EditAnywhere, Category="Combat")
	class UAnimMontage* HitReactionMontage;

	// 死亡动画蒙太奇
	UPROPERTY(EditAnywhere, Category="Combat")
	class UAnimMontage* ElimMontage;
	
	UPROPERTY(EditAnywhere, Category="Camera")
	float CameraThreshold = 200.0f;

	bool bRotateRootBone;

	// 最大生命值
	UPROPERTY(EditAnywhere, Category = "Player State")
	float MaxHealth = 100.0f;

	UPROPERTY(ReplicatedUsing=OnRep_Health, VisibleAnywhere, Category="Player State")
	float Health = 100.0f;

	class ABlasterPlayerController* BlasterPlayerController;

	bool bElimmed = false;

	FTimerHandle ElimTimer;

	UPROPERTY(EditAnywhere, Category="Elim")
	float ElimDelay = 3.0f;
	
	void ElimTimerFinished();

	/*
	 * 溶解效果
	 */
	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimelineComponent;
	
	FOnTimelineFloat DissolveTrack;

	UPROPERTY(EditAnywhere)
	// 溶解曲线
	UCurveFloat* DissolveCurve;

	UFUNCTION()
	void UpdateDissolveMeterial(float DissolveValue);

	void StartDissolve();

	// 动态材质实例
	UPROPERTY(VisibleAnywhere, Category="Elim")
	UMaterialInstanceDynamic* DynamicDissolveMaterialInstance;

	UPROPERTY(VisibleAnywhere, Category="Elim")
	UMaterialInstance* DissolverMaterialInstance;

	class ABlasterPlayerState* BlasterPlayerState;
	
public:

	
	void SetOverlappingWeapon(AWeapon* Weapon);

	bool IsWeaponEquipped();

	bool IsAiming();

	AWeapon* GetEquippedWeapon();

	FVector GetHitTarget() const;

	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }

	FORCEINLINE float GetAO_Yaw() const {return AO_Yaw;}
	
	FORCEINLINE float GetAO_Pitch() const {return AO_Pitch;}
	
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }

	FORCEINLINE bool IsElimmed() const { return bElimmed; }

	FORCEINLINE float GetHealth() const { return Health; }
	
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

	ECombatState GetCombatState() const;
	
};
