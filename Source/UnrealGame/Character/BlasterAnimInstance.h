// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "UnrealGame/Enums/UnrealGameEnumInterface.h"
#include "BlasterAnimInstance.generated.h"


/*
 * @name: EMovementDirection
 * @description: 玩家速度方向 - 八方向
 * 
 * @author: yejianbin
 * @createTime: 2022年06月19日 星期日
 */
UENUM(BlueprintType)
enum class EMovementDirection : uint8
{
	EMD_Idle UMETA(DisplayName="Idle"),
	EMD_Forward UMETA(DisplayName="Forward"),
	EMD_Backward UMETA(DisplayName="Backward"),
	EMD_Leftward UMETA(DisplayName="Leftward"),
	EMD_Rightward UMETA(DisplayName="Rightward"),
	EMD_FL UMETA(DisplayName="FL"),
	EMD_FR UMETA(DisplayName="FR"),
	EMD_BL UMETA(DisplayName="BL"),
	EMD_BR UMETA(DisplayName="BR")
};

/*
 * @name: BlasterAnimInstance - 冲击波动画实例类
 * @description: 冲击波动画实例类
 * 
 * @author: yejianbin
 * @createTime: 2022年06月19日
 */
UCLASS()
class UNREALGAME_API UBlasterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	// 类似 BeginPlayer()
	virtual void NativeInitializeAnimation() override;

	// 类似 Tick()
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	/*
	 * @description: 初始化动画实例所需的引用
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 9:36:55
	 */
	void InitialReferences();

	/*
	 * @description: 更新属性
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年06月19日 星期日 09:06:28
	 */
	void UpdateProperties();

	/*
	 * @description: 更新状态
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年06月19日 星期日 09:06:28
	 */
	void UpdateState();

private:

	UPROPERTY(BlueprintReadOnly, Category = "References", meta=(AllowPrivateAccess=true), DisplayName="玩家角色")
	class ABlasterCharacter* BlasterCharacter;

	// 是否在空中
	UPROPERTY(BlueprintReadOnly, Category = "Movement States", meta=(AllowPrivateAccess=true), AdvancedDisplay, DisplayName="是否在空中")
	bool bIsInAir;

	// 是否正在跳跃
	UPROPERTY(BlueprintReadOnly, Category = "Movement States", meta=(AllowPrivateAccess=true), AdvancedDisplay, DisplayName="是否跳跃中")
	bool bIsJumping; 

	// 是否正在加速
	UPROPERTY(BlueprintReadOnly, Category = "Movement States", meta=(AllowPrivateAccess=true), AdvancedDisplay, DisplayName="是否正在加速")
	bool bIsAccelerating; 

	// 是否正在蹲伏
	UPROPERTY(BlueprintReadOnly, Category= "Movement States", meta=(AllowPrivateAccess=true), AdvancedDisplay, DisplayName="是否正在蹲伏中")
	bool bIsCrouched;

	// 是否正在瞄准
	UPROPERTY(BlueprintReadOnly, Category= "Movement States", meta=(AllowPrivateAccess=true), AdvancedDisplay, DisplayName="是否在瞄准")
	bool bIsAiming;

	// 是否死亡
	UPROPERTY(BlueprintReadOnly, Category="Movement States", meta=(AllowPrivateAccess=true), AdvancedDisplay, DisplayName="是否死亡")
	bool bElimmed;

	// 速度方向
	UPROPERTY(BlueprintReadOnly, Category="Movement Properties", meta=(AllowPrivateAccess=true), DisplayName="速度方向")
	EMovementDirection MovementDirection;

	// 速度值
	UPROPERTY(BlueprintReadOnly, Category = "Movement Properties", meta=(AllowPrivateAccess=true), DisplayName="移动速度")
	float Speed;

	// 跳跃到降落地面的过渡值 - TODO
	UPROPERTY(BlueprintReadOnly, Category = "Movement Properties", meta=(AllowPrivateAccess=true), DisplayName="跳跃到降落地面的过渡值")
	float JumpToGroundBlend;

	// Walk / Jog 速度等级
	UPROPERTY(BlueprintReadOnly, Category = "Movement Properties", meta=(AllowPrivateAccess=true), DisplayName="行走/奔跑的速度等级")
	float MovementSpeedLevel;
	
	UPROPERTY(BlueprintReadOnly, Category = "Movement Properties", meta=(AllowPrivateAccess=true), DisplayName="瞄准偏移-Yaw")
	float AO_YawOffset;

	UPROPERTY(BlueprintReadOnly, Category = "Movement Properties", meta=(AllowPrivateAccess=true), DisplayName="瞄准偏移-Pitch")
	float AO_PitchOffset;

	UPROPERTY(BlueprintReadOnly, Category = "Movement Properties", meta=(AllowPrivateAccess=true), DisplayName="瞄准偏移混合值")
	float AO_Blend;

	UPROPERTY(BlueprintReadOnly, Category = "Equipment", meta=(AllowPrivateAccess=true), DisplayName="玩家装备状态")
	EPlayerEquipState PlayerEquipState;
};
