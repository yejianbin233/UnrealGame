// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlasterAnimInstance.h"
#include "InputActionValue.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UnrealGame/Backpack/BackpackLagCompensationComponent.h"
#include "UnrealGame/Enums/UnrealGameEnumInterface.h"
#include "UnrealGame/Enums/WeaponTypes.h"
#include "UnrealGame/Backpack/ItemInfoObject.h"
#include "UnrealGame/Component/Camera/UnrealCameraComponent.h"
#include "UnrealGame/Struct/UnrealGameStruct.h"
#include "BlasterCharacter.generated.h"


class USpringArmComponent;
class UCameraComponent;
/*
 * FVelocityBlend 根据速度来判断方向混合量
 */
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
class UNREALGAME_API ABlasterCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	/* Enhanced Input  */
	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputMappingContext")
	TObjectPtr<class UInputMappingContext> ShootGameInputMappingContext;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="射击")
	TObjectPtr<class UInputAction> EIA_Shoot;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="连续射击")
	TObjectPtr<class UInputAction> EIA_ShootHold;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="射击停止")
	TObjectPtr<class UInputAction> EIA_ShootButtonReleased;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="瞄准")
	TObjectPtr<UInputAction> EIA_Aim;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="取消瞄准")
	TObjectPtr<UInputAction> EIA_UnAim;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="蹲伏")
	TObjectPtr<UInputAction> EIA_Crouch;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="丢弃")
	TObjectPtr<UInputAction> EIA_Discard;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="拾取")
	TObjectPtr<UInputAction> EIA_Pickup;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="装备")
	TObjectPtr<UInputAction> EIA_Equipment;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="取消装备")
	TObjectPtr<UInputAction> EIA_UnEquipment;

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

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="打开摄像机")
	TObjectPtr<UInputAction> EIA_OpenCamera;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", AdvancedDisplay, DisplayName="进入摄像机调试模式")
	TObjectPtr<class UInputAction> EIA_EnterDebugMode;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", AdvancedDisplay, DisplayName="退出摄像机调试模式")
	TObjectPtr<class UInputAction> EIA_LeaveDebugMode;
	/* Enhanced Input  */
	
private:

	/*
	 * 组件
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera", DisplayName="摄像机组件", meta=(AllowPrivateAccess=true))
	class UUnrealCameraComponent* PlayerCameraComponent;

	UPROPERTY(BlueprintReadOnly,Category="Pickup", DisplayName="是否检测到附近有可拾取物品", meta=(AllowPrivateAccess=true))
	bool bHasPickableObject;

	UPROPERTY(BlueprintReadOnly, Category="Pickup Object", meta=(AllowPrivateAccess=true), DisplayName="当前可拾取的物品数组", meta=(AllowPrivateAccess=true))
	TArray<AItemBase*> PickableObjects;
	// 头顶控件组件
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true), DisplayName="头部控件组件")
	class UWidgetComponent* OverheadWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName="背包组件", meta=(AllowPrivateAccess=true))
	class UBackpackComponent* BackpackComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName="背包滞后补偿组件", meta=(AllowPrivateAccess=true))
	class UBackpackLagCompensationComponent* BackpackLagCompensationComponent;

	/* 玩家角色滞后补偿相关设置 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, DisplayName="玩家角色滞后补偿组件", meta=(AllowPrivateAccess=true))
	class UPlayerLagCompensationComponent* PlayerLagCompensationComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, DisplayName="玩家滞后补偿骨骼碰撞组件标签名", meta=(AllowPrivateAccess=true))
	FName BoneCollisionBoxTagName=TEXT("BoneCollisionBox");
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Collision Box", meta=(AllowPrivateAccess=true))
	class UBoxComponent* HeadBoxComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Collision Box", meta=(AllowPrivateAccess=true))
	class UBoxComponent* Spine_05_BoxComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Collision Box", meta=(AllowPrivateAccess=true))
	class UBoxComponent* Spine_03_BoxComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Collision Box", meta=(AllowPrivateAccess=true))
	class UBoxComponent* Spine_01_BoxComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Collision Box", meta=(AllowPrivateAccess=true))
	class UBoxComponent* Left_Clavicle_Out_BoxComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Collision Box", meta=(AllowPrivateAccess=true))
	class UBoxComponent* Left_Upperarm_BoxComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Collision Box", meta=(AllowPrivateAccess=true))
	class UBoxComponent* Left_Lowerarm_BoxComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Collision Box", meta=(AllowPrivateAccess=true))
	class UBoxComponent* Left_Hand_BoxComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Collision Box", meta=(AllowPrivateAccess=true))
	class UBoxComponent* Left_Thigh_BoxComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Collision Box", meta=(AllowPrivateAccess=true))
	class UBoxComponent* Left_Calf_BoxComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Collision Box", meta=(AllowPrivateAccess=true))
	class UBoxComponent* Left_Foot_BoxComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Collision Box", meta=(AllowPrivateAccess=true))
	class UBoxComponent* Right_Clavicle_Out_BoxComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Collision Box", meta=(AllowPrivateAccess=true))
	class UBoxComponent* Right_Upperarm_BoxComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Collision Box", meta=(AllowPrivateAccess=true))
	class UBoxComponent* Right_Lowerarm_BoxComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Collision Box", meta=(AllowPrivateAccess=true))
	class UBoxComponent* Right_Hand_BoxComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Collision Box", meta=(AllowPrivateAccess=true))
	class UBoxComponent* Right_Thigh_BoxComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Collision Box", meta=(AllowPrivateAccess=true))
	class UBoxComponent* Right_Calf_BoxComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Collision Box", meta=(AllowPrivateAccess=true))
	class UBoxComponent* Right_Foot_BoxComponent;
	
	/* 玩家角色滞后补偿相关设置 */
	
	// 战斗组件
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Component", DisplayName="战斗组件", meta=(AllowPrivateAccess=true))
	class UCombatComponent* CombatCopmponent;
	
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
	UPROPERTY(Replicated, VisibleAnywhere, Category="Elimmed", DisplayName="是否被淘汰")
	bool bElimmed = false;
	
	FTimerHandle ElimTimer;

	UPROPERTY(EditAnywhere, Category="Elim")
	float ElimDelay = 3.0f;

	/*
	 * 溶解效果
	 */
	UPROPERTY(VisibleAnywhere, DisplayName="溶解时间线组件")
	UTimelineComponent* DissolveTimelineComponent;

	// 溶解时间线组件委托
	FOnTimelineFloat DissolveTrack;

	UPROPERTY(EditAnywhere, DisplayName="溶解曲线")
	UCurveFloat* DissolveCurve;

	// 动态材质实例
	UPROPERTY(VisibleAnywhere, Category="Elim")
	UMaterialInstanceDynamic* DynamicDissolveMaterialInstance;

	UPROPERTY(VisibleAnywhere, Category="Elim")
	UMaterialInstance* DissolverMaterialInstance;

	UPROPERTY()
	class ABlasterPlayerState* BlasterPlayerState;

	UPROPERTY()
	class ABlasterPlayerController* BlasterPlayerController;

	/* 
	 * 控制鼠标转向移动速度、鼠标上下视角移动速度 可以"弃用"，因为 PlayerController 原本就有属性控制，诸如：InputPitchScale_DEPRECATED 等。
	 *
	 * PlayerControler 控制开关在配置文件 "DefaultInput" 的 bEnableLegacyInputScales 控制。
	 *
	 * AddControllerYawInput 等函数实现使用相关函数
	 */
	// 鼠标转向移动速度控制
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category="Mouse Settings", meta=(AllowPrivateAccess=true), DisplayName="控制鼠标左右转向变化速率")
	float MouseTurnRate = 1.0f;

	// 鼠标上下视角移动速度控制
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category="Mouse Settings", meta=(AllowPrivateAccess=true), DisplayName="控制鼠标俯仰视变化速率")
	float MouseLookupRate = 1.0f;

	// 用于禁止玩家移动输入(不包括旋转)和装备武器
	UPROPERTY(Replicated)
	bool bDisableGameplay = false;

	// 玩家速度方向
	UPROPERTY(Replicated, BlueprintReadOnly, Category="Movement States", meta=(AllowPrivateAccess=true), DisplayName="速度移动方向")
	EMovementDirection MovementDirection;

	UPROPERTY(Replicated, BlueprintReadOnly, Category="Movement States", meta=(AllowPrivateAccess=true), DisplayName="是否跳跃中")
	bool bIsJump;

	UPROPERTY(BlueprintReadWrite, Category="Movement Properties", meta=(AllowPrivateAccess=true), DisplayName="跳跃到地面的过渡混合值")
	float JumpToGroundBlend;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement Properties", meta=(AllowPrivateAccess=true), DisplayName="跳跃到地面的过渡距离")
	float JumpToGroundTraceDistance = 20.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement Properties", meta=(AllowPrivateAccess=true), DisplayName="行走移动速度")
	float WalkSpeed=300.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement Properties", meta=(AllowPrivateAccess=true), DisplayName="加速移动速度")
	float SprintSpeed=650.0f;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category="Movement Properties", meta=(AllowPrivateAccess=true))
	float MovementSpeedLevel;

	// 最大向上仰视角度 0-180
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement Properties", meta=(AllowPrivateAccess=true), DisplayName="最大向上仰视角度 0-180")
	float MaxTopPitch = 40;

	// 最大向下俯视角度 0-180
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement Properties", meta=(AllowPrivateAccess=true), DisplayName="最大向下俯视角度 0-180")
	float MaxLowPitch = 60;

	UPROPERTY(Replicated, BlueprintReadOnly, Category="Movement Properties", meta=(AllowPrivateAccess=true))
	float AO_PitchOffset;

	UPROPERTY(Replicated, BlueprintReadOnly, Category="Movement Properties", meta=(AllowPrivateAccess=true))
	float AO_YawOffset;

	UPROPERTY(Replicated, BlueprintReadOnly, Category="Movement Properties", meta=(AllowPrivateAccess=true))
	float AO_Blend;

	UPROPERTY(EditAnywhere, Category="Movement Properties", meta=(AllowPrivateAccess=true))
	float AO_Blend_Speed = 1;
	
	UPROPERTY(EditAnywhere, Category="Pickup Properties", meta=(AllowPrivateAccess=true), DisplayName="可拾取检测起点偏移值")
	float PickupTraceStartOffset = 100;
	
	UPROPERTY(EditAnywhere, Category="Pickup Properties", meta=(AllowPrivateAccess=true), DisplayName="可拾取检测半径")
	float PickupTraceRadius = 10;
	
	UPROPERTY(EditAnywhere, Category="Pickup Properties", meta=(AllowPrivateAccess=true), DisplayName="可拾取检测距离")
	float PickupTraceDistance = 100;

	UPROPERTY(EditAnywhere, Category="Pickup Properties", meta=(AllowPrivateAccess=true), DisplayName="可拾取检测物品类型")
	TEnumAsByte<ETraceTypeQuery> PickableTraceTypeQuery;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Interactive", meta=(AllowPrivateAccess=true), DisplayName="可交互的距离")
	float InteractiveTraceDistance = 100;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Interactive", meta=(AllowPrivateAccess=true), DisplayName="可交互的检测对象")
	TEnumAsByte<ETraceTypeQuery> InteractiveTraceType;
	
public:

	// Sets default values for this character's properties
	ABlasterCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PostInitializeComponents() override;

	// 播放受击动画蒙太奇
	void PlayHitReactMontage();

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
	struct FItemInfo GetItemInfoFromTable(FName Id);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Interactive", DisplayName="交互")
	void Interactive();

	UFUNCTION(BlueprintCallable, Category="Pickup", DisplayName="检测可拾取对象")
	AItemBase* TracePickableObject(EPickableObjectState PickableObjectState);

	// TODO 检测到可拾取物体时，高亮显示
	void HighLightPickableObject();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Equipment", DisplayName="获取装备Actor类")
	FEquipmentInfo GetEquipmentActorClass(FName EquipmentActorName);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/*
	 * 玩家输入处理函数
	 */
	// 前后移动输入
	void MoveForward(const FInputActionValue& ActionValue);

	// 左右移动输入
	void MoveRight(const FInputActionValue& ActionValue);

	// 转向(鼠标X轴)输入
	void Turn(const FInputActionValue& ActionValue);

	// 上下俯仰视(鼠标Y轴)输入
	void LookUp(const FInputActionValue& ActionValue);

	// 跳跃
	void JumpButtonPressed();

	// 装备武器
	UFUNCTION(Category="Equipment")
	void EquipButtonPressed();

	// 取消装备武器
	UFUNCTION(Category="Equipment")
	void UnEquipButtonPressed();

	// 蹲伏
	void CrouchButtonPressed();

	// 加速
	void SprintButtonPressed();

	// 停止加速
	void SprintButtonReleased();

	// 瞄准
	void AimButtonPressed();

	// 取消瞄准
	void AimButtonReleased();

	// 开火
	void FireButtonPressed();
	
	// 连续开火
	void FireHold();

	// 取消开火
	void FireButtonReleased();

	// 装填子弹
	void ReloadButtonPressed();

	// 打开或关闭背包
	void OpenOrCloseBackpack();
	/*
	 * @description: RotateDragItemWidget - 用于处理用户输入，旋转正在拖动的物品控件
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月08日 星期五 19:07:10
	 */
	void RotateDragItemWidget();

	/*
	 * @description: Pickup 拾取 - 客户端拾取
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月27日 星期三 10:07:22
	 */
	void Pickup();
	
	UFUNCTION(Client, Reliable, Category="Pickup", DisplayName="客户端拾取")
	void CC_Pickup();

	UFUNCTION(Server, Reliable, Category="Pickup", DisplayName="服务器拾取")
	void SC_Pickup(AItemBase* PickupedUpItem, float BackpackItemChangedTime);

	/* 玩家输入处理函数 */

	// 收到伤害处理方法
	UFUNCTION(DisplayName="接收伤害")
	void ReceiveDamage(AActor* DamagedActor
		, float Damage
		, const UDamageType* DamageType
		, AController* InstigatorController
		, AActor* DamagerCauser);

	// 更新 Hud
	void UpdateHUDHealth();

	// 玩家 Playerstate
	void PollInit();
	
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

public:

	// TODO - 优化，处理动画蓝图中跳跃到地面的过渡动画
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateJumpToGroundBlend();

	UFUNCTION(BlueprintImplementableEvent)
	void PickableObjectTrace(FVector Start, FVector End, FHitResult HitResult);

	/* Weapon 武器 */
	void SetOverlappingWeapon(AWeapon* Weapon);

	bool IsWeaponEquipped();

	AWeapon* GetEquippedWeapon();

	ECombatState GetCombatState() const;

	FORCEINLINE UCombatComponent* GetCombatComponent() const { return CombatCopmponent; }

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

	FORCEINLINE UCameraComponent* GetFollowCamera() const { return PlayerCameraComponent->GetCameraComponent(); }
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return PlayerCameraComponent->GetSpringArmComponent(); }
	FORCEINLINE UUnrealCameraComponent* GetPlayerCameraComponent() const { return PlayerCameraComponent; }

	/* Character Properties*/
	
	FORCEINLINE float GetHealth() const { return Health; }
	
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	
	/* Character Properties*/
	
	FORCEINLINE float GetDisableGameplay() const { return bDisableGameplay; }

	FORCEINLINE UBackpackComponent* GetBackpackComponent() const { return BackpackComponent; };
	FORCEINLINE UBackpackLagCompensationComponent* GetBackpackLagCompensationComponent() const { return BackpackLagCompensationComponent; };

	FORCEINLINE FName GetBoneCollisionBoxTagName() const { return BoneCollisionBoxTagName; };

	FORCEINLINE UPlayerLagCompensationComponent* GetPlayerLagCompensationComponent() const { return PlayerLagCompensationComponent; };

	FORCEINLINE TArray<AItemBase*> GetPickableObjects() const { return PickableObjects; };






















	
	// ===================================================
	

	static void DisplayRole(ENetRole Role);
};
