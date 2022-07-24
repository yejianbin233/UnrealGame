// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlasterAnimInstance.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UnrealGame/Enums/UnrealGameEnumInterface.h"
#include "UnrealGame/Enums/WeaponTypes.h"
#include "UnrealGame/HUD/Backpack/ItemInfoObject.h"
#include "UnrealGame/Interfaces/InteractWithCrosshairsInterface.h"
#include "BlasterCharacter.generated.h"


struct FInputActionValue;

USTRUCT(BlueprintType)
struct FVelocityBlend
{
	GENERATED_BODY()

public:

	float Forward;
	float Backward;
	float Leftward;
	float Rightward;

	FORCEINLINE void UpdateBlendValue(float ForwardValue, float BackwardValue, float LeftwardValue, float RightwardValue)
	{
		this->Forward = ForwardValue;
		this->Backward = BackwardValue;
		this->Leftward = LeftwardValue;
		this->Rightward = RightwardValue;
	};
};

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

	/* 动画蒙太奇 */
	// 播放开火动画蒙太奇
	void PlayFireMontage(bool bIsAiming);

	// 播放受击动画蒙太奇
	void PlayHitReactMontage();

	// 播放重装弹蒙太奇
	void PlayReloadMontage();

	// 播放死亡动画蒙太奇
	UFUNCTION(NetMulticast, Reliable)
	void PlayElimMontage();
	/* 动画蒙太奇 */

	// 死亡处理
	void Elim();


	/*
	 * @description:GetItemInfoFromTable - 从物品数据表获取物品数据
	 * @param Id - 物品的唯一Id
	 *	
	 * @return FItemInfo - 物品数据结构
	 * ...
	 *
	 *	note：TODO 临时放置，便于获取数据表
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月08日 星期五 19:07:52
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="背包组件", DisplayName="根据物品Id从物品表获取物品数据")
	FItemInfo GetItemInfoFromTable(FName Id);

	UFUNCTION(BlueprintCallable, Category="Interactive", DisplayName="交互")
	void Interactive();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/*
	 * 玩家输入处理函数
	 */
	void MoveForward(const FInputActionValue& ActionValue);

	void MoveRight(const FInputActionValue& ActionValue);

	void Turn(const FInputActionValue& ActionValue);

	void LookUp(const FInputActionValue& ActionValue);

	void JumpButtonPressed();
	
	void EquipButtonPressed();

	void CrouchButtonPressed();
	
	void SprintButtonPressed();
	
	void SprintButtonReleased();

	void AimButtonPressed();
	
	void AimButtonReleased();

	void FireButtonPressed();
	
	void FireButtonReleased();

	void ReloadButtonPressed();

	void OpenOrCloseBackpack();

	/*
	 * @description: RotateDragItemWidget - 用于处理用户输入，旋转正在拖动的物品控件
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月08日 星期五 19:07:10
	 */
	void RotateDragItemWidget();
	
	void Pickup();
	
	/* 玩家输入处理函数 */

	// 收到伤害处理方法
	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor
		, float Damage
		, const UDamageType* DamageType
		, AController* InstigatorController
		, AActor* DamagerCauser);

	// 更新 Hud
	void UpdateHUDHealth();

	// 玩家 Playerstate
	void PollInit();


public:
	UPROPERTY(BlueprintReadOnly, Replicated, Category="Pickup", DisplayName="是否检测到附近有可拾取物品")
	bool bHasPickableObject;
	
private:

	/*
	 * 组件
	 */

	// 弹簧臂组件
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class USpringArmComponent* CameraBoom;

	// 摄像机组件
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class UCameraComponent* FollowCamera;

	// 头顶控件组件
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	class UWidgetComponent* OverheadWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName="背包组件", meta=(AllowPrivateAccess=true))
	class UBackpackComponent* BackpackComponent;

	// 武器组件
	UPROPERTY(ReplicatedUsing="OnRep_OverlappingWeapon")
	class AWeapon* OverlappingWeapon;

	// 战斗组件
	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* Combat;

	

	/**
	 * 成员属性
	 */

	// 开火蒙太奇
	UPROPERTY(EditAnywhere, Category="Combat")
	UAnimMontage* FireWeaponMontage;

	// 重新装弹蒙太奇
	UPROPERTY(EditAnywhere, Category="Combat")
	UAnimMontage* ReloadMontage;
	// 被击打动画蒙太奇
	UPROPERTY(EditAnywhere, Category="Combat")
	UAnimMontage* HitReactionMontage;

	// 死亡动画蒙太奇
	UPROPERTY(EditAnywhere, Category="Combat")
	UAnimMontage* ElimMontage;
	
	UPROPERTY(EditAnywhere, Category="Camera")
	float CameraThreshold = 200.0f;

	// 最大生命值
	UPROPERTY(EditAnywhere, Category = "Player State")
	float MaxHealth = 100.0f;

	// 生命值
	UPROPERTY(ReplicatedUsing=OnRep_Health, VisibleAnywhere, Category="Player State")
	float Health = 100.0f;

	// 是否死亡
	bool bElimmed = false;
	
	FTimerHandle ElimTimer;

	UPROPERTY(EditAnywhere, Category="Elim")
	float ElimDelay = 3.0f;

	/*
	 * 溶解效果
	 */
	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimelineComponent;
	
	FOnTimelineFloat DissolveTrack;

	UPROPERTY(EditAnywhere)
	// 溶解曲线
	UCurveFloat* DissolveCurve;
	

	// 动态材质实例
	UPROPERTY(VisibleAnywhere, Category="Elim")
	UMaterialInstanceDynamic* DynamicDissolveMaterialInstance;

	UPROPERTY(VisibleAnywhere, Category="Elim")
	UMaterialInstance* DissolverMaterialInstance;

	class ABlasterPlayerState* BlasterPlayerState;

	class ABlasterPlayerController* BlasterPlayerController;

	/* Enhanced Input  */
	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputMappingContext")
	TObjectPtr<class UInputMappingContext> ShootGameInputMappingContext;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="射击")
	TObjectPtr<class UInputAction> EIA_Shoot;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="瞄准")
	TObjectPtr<UInputAction> EIA_Aim;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="蹲伏")
	TObjectPtr<UInputAction> EIA_Crouch;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="丢弃")
	TObjectPtr<UInputAction> EIA_Discard;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="拾取")
	TObjectPtr<UInputAction> EIA_Pickup;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="装备")
	TObjectPtr<UInputAction> EIA_Equipment;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="跳跃")
	TObjectPtr<UInputAction> EIA_Jump;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="向前移动")
	TObjectPtr<UInputAction> EIA_MoveForward;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="向右移动")
	TObjectPtr<UInputAction> EIA_MoveRight;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="装填弹药")
	TObjectPtr<UInputAction> EIA_ReloadAmmo;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="冲刺")
	TObjectPtr<UInputAction> EIA_Sprint;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="不冲刺")
	TObjectPtr<UInputAction> EIA_UnSprint;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="切换")
	TObjectPtr<UInputAction> EIA_UseSwitch;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="转身")
	TObjectPtr<UInputAction> EIA_Turn;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="俯仰视")
	TObjectPtr<UInputAction> EIA_Lookup;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="打开或关闭背包")
	TObjectPtr<UInputAction> EIA_OpenOrCloseBackpack;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="旋转背包物品")
	TObjectPtr<UInputAction> EIA_RotateDragItemWidget;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="交互")
	TObjectPtr<UInputAction> EIA_Interactive;
	/* Enhanced Input  */


	/* 
	 * 控制鼠标转向移动速度、鼠标上下视角移动速度 可以"弃用"，因为 PlayerController 原本就有属性控制，诸如：InputPitchScale_DEPRECATED 等。
	 *
	 * PlayerControler 控制开关在配置文件 "DefaultInput" 的 bEnableLegacyInputScales 控制。
	 *
	 * AddControllerYawInput 等函数实现使用相关函数
	 */
	// 鼠标转向移动速度控制
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category="Mouse Settings", meta=(AllowPrivateAccess))
	float MouseTurnRate = 1.0f;

	// 鼠标上下视角移动速度控制
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category="Mouse Settings", meta=(AllowPrivateAccess))
	float MouseLookupRate = 1.0f;

	// 用于禁止玩家移动输入(不包括旋转)和装备武器
	UPROPERTY(Replicated)
	bool bDisableGameplay = false;

	// 玩家速度方向
	UPROPERTY(BlueprintReadOnly, Category="Movement States", meta=(AllowPrivateAccess))
	EMovementDirection MovementDirection;

	UPROPERTY(BlueprintReadOnly, Category="Movement States", meta=(AllowPrivateAccess))
	bool bIsJump;

	UPROPERTY(BlueprintReadWrite, Category="Movement Properties", meta=(AllowPrivateAccess))
	float JumpToGroundBlend;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement Properties", meta=(AllowPrivateAccess))
	float JumpToGroundTraceDistance = 20.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement Properties", meta=(AllowPrivateAccess))
	float WalkSpeed=300.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement Properties", meta=(AllowPrivateAccess))
	float SprintSpeed=650.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement Properties", meta=(AllowPrivateAccess))
	float MovementSpeedLevel;

	// 最大向上仰视角度 0-180
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement Properties", meta=(AllowPrivateAccess))
	float MaxTopPitch = 40;

	// 最大向下俯视角度 0-180
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement Properties", meta=(AllowPrivateAccess))
	float MaxLowPitch = 60;

	UPROPERTY(BlueprintReadOnly, Category="Movement Properties", meta=(AllowPrivateAccess))
	float AO_PitchOffset;

	UPROPERTY(BlueprintReadOnly, Category="Movement Properties", meta=(AllowPrivateAccess))
	float AO_YawOffset;

	UPROPERTY(BlueprintReadOnly, Category="Movement Properties", meta=(AllowPrivateAccess))
	float AO_Blend;

	UPROPERTY(EditAnywhere, Category="Movement Properties", meta=(AllowPrivateAccess))
	float AO_Blend_Speed = 1;
	
	UPROPERTY(EditAnywhere, Category="Pickup Properties", meta=(AllowPrivateAccess))
	float PickupTraceStartOffset = 100;
	
	UPROPERTY(EditAnywhere, Category="Pickup Properties", meta=(AllowPrivateAccess))
	float PickupTraceRadius = 10;
	
	UPROPERTY(EditAnywhere, Category="Pickup Properties", meta=(AllowPrivateAccess))
	float PickupTraceDistance = 100;

	UPROPERTY(EditAnywhere, Category="Pickup Properties", meta=(AllowPrivateAccess))
	TEnumAsByte<ETraceTypeQuery> PickableTraceTypeQuery;

	UPROPERTY(BlueprintReadOnly, Category="Pickup Object", meta=(AllowPrivateAccess))
	AActor* CurrentPickableActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Interactive", meta=(AllowPrivateAccess), DisplayName="可交互的距离")
	float InteractiveTraceDistance=100;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Interactive", meta=(AllowPrivateAccess), DisplayName="可交互的检测对象")
	TEnumAsByte<ETraceTypeQuery> InteractiveTraceType;
	
private:
	/*
	 * 函数
	 */
	
	UFUNCTION()
	void UpdateDissolveMeterial(float DissolveValue);

	// 开始溶解
	void StartDissolve();

	// 死亡时间处理器
	void ElimTimerFinished();

	// 当摄像机接近网格体时使其隐藏
	void HideCameraIfCharacterClose();

	UFUNCTION()
	void OnRep_Health();

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);
	
	UFUNCTION(Server, Reliable)
	void ServerEquipButtomPressed();

	/*
	 * @description: 更新玩家的速度方向 - 八方向
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年06月18日
	 */
	void UpdateMovementDirection();

	void UpdateJumpState();

	void UpdateMovementSpeedLevel();

	/*
	 * @description: 在移动和空闲状态下，切换不同的旋转模式
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年06月19日 星期日
	 */
	void UpdateMovementRotation();

	void UpdateAimOffset(float DeltaTime);

	void TracePickableOjbect();

public:
	/*
		 * @description:
		 * @param
		 * @return
		 * ...
		 * 
		 * @author: yejianbin
		 * @version: v1.0
		 * @createTime: 2022年06月19日 星期日 12:06:22
		 */
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateJumpToGroundBlend();

	UFUNCTION(BlueprintImplementableEvent)
	void PickableObjectTrace(FVector Start, FVector End, FHitResult HitResult);
	/* Weapon 武器 */
	void SetOverlappingWeapon(AWeapon* Weapon);

	bool IsWeaponEquipped();

	AWeapon* GetEquippedWeapon();

	ECombatState GetCombatState() const;

	FORCEINLINE UCombatComponent* GetCombat() const { return Combat; }

	/* Weapon 武器*/

	/* Movement States */
	bool IsAiming();

	FORCEINLINE bool IsElimmed() const { return bElimmed; }
	
	FORCEINLINE bool IsInAir() const { return GetCharacterMovement()->IsFalling(); };
	
	FORCEINLINE bool IsJumping() const { return bIsJump; };
	
	FORCEINLINE bool IsAccelerating() const { return GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.0f ? true : false; };
	/* Movement States*/

	/* Movement Properties*/

	FORCEINLINE EMovementDirection GetMovementDirection() const { return MovementDirection; };
	
	FORCEINLINE float GetJumpToGroundBlend() const { return JumpToGroundBlend; };
	
	FORCEINLINE float GetMovementSpeedLevel() const { return MovementSpeedLevel; };
	
	FORCEINLINE float GetAO_YawOffset() const { return AO_YawOffset; };
	
	FORCEINLINE float GetAO_PitchOffset() const { return AO_PitchOffset; };
	
	FORCEINLINE float GetAO_Blend() const { return AO_Blend; };

	/* Movement Properties*/

	FVector GetHitTarget() const;

	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/* Character Properties*/
	
	FORCEINLINE float GetHealth() const { return Health; }
	
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	
	/* Character Properties*/
	
	FORCEINLINE float GetDisableGameplay() const { return bDisableGameplay; }
};
