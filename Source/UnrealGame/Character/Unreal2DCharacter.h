// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "PaperCharacter.h"
#include "PaperFlipbook.h"
#include "Unreal2DCharacter.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EFlipBookState : uint8
{
	Transitioning UMETA(DisplayName="过渡状态，如果没有主动切换其他持久状态时应切换到该状态"),
	GroundIdle UMETA(DisplayName="地面空闲状态"),
	GroundMovement UMETA(DisplayName="地面运动状态"),
	Jumping UMETA(DisplayName="跳跃状态"),
	Falling UMETA(DisplayName="下落状态"),
	Climbing UMETA(DisplayName="攀爬状态"),
	TemporaryAnimation UMETA(DisplayName="临时状态动画，如释放技能，死亡等只播放一次"),
};

UENUM(BlueprintType)
enum class EFlipBookTemporaryAnimation : uint8
{
	GroundCrouch UMETA(DisplayName="地面蹲伏动画"),
	GroundDash UMETA(DisplayName="地面猛冲动画"),
	GroundDashAttack UMETA(DisplayName="地面猛冲攻击动画"),
	Death UMETA(DisplayName="死亡动画"),
	EdgeGrab UMETA(DisplayName="边缘抓住动画"),
	Hurted UMETA(DisplayName="受伤动画"),
	Slide UMETA(DisplayName="滑动动画"),
	Attack UMETA(DisplayName="攻击动画"),
};



UCLASS()
class UNREALGAME_API AUnreal2DCharacter : public APaperCharacter
{
	GENERATED_BODY()

public:

	AUnreal2DCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/* Enhanced Input  */
	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputMappingContext")
	TObjectPtr<class UInputMappingContext> Unreal2DGameInputMappingContext;
	
	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="向前移动")
	TObjectPtr<UInputAction> EIA_MoveForward;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="向后移动")
	TObjectPtr<UInputAction> EIA_MoveBackward;
	
	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="蹲伏")
	TObjectPtr<UInputAction> EIA_Crouch;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="取消蹲伏")
	TObjectPtr<UInputAction> EIA_UnCrouch;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="向上攀爬")
	TObjectPtr<UInputAction> EIA_ClimbUp;
	
	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="向下攀爬")
	TObjectPtr<UInputAction> EIA_ClimbDown;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="跳跃")
	TObjectPtr<UInputAction> EIA_Jump;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="攻击")
	TObjectPtr<UInputAction> EIA_Attack;
	
	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="猛冲")
	TObjectPtr<UInputAction> EIA_Dash;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="滑动")
	TObjectPtr<UInputAction> EIA_Slide;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category="Flipbook", DisplayName="空闲动画")
	TObjectPtr<UPaperFlipbook> FB_Idle;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category="Flipbook", DisplayName="奔跑动画")
	TObjectPtr<UPaperFlipbook> FB_Run;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category="Flipbook", DisplayName="跳跃动画")
	TObjectPtr<UPaperFlipbook> FB_Jump;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category="Flipbook", DisplayName="下落动画")
	TObjectPtr<UPaperFlipbook> FB_Falling;

	// ----- Temporary State（临时）
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category="Flipbook", DisplayName="蹲伏动画")
	TObjectPtr<UPaperFlipbook> FB_Crouch;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category="Flipbook", DisplayName="蹲伏姿势")
	TObjectPtr<UPaperFlipbook> FB_CrouchPose;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category="Flipbook", DisplayName="猛冲动画")
	TObjectPtr<UPaperFlipbook> FB_Dash;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category="Flipbook", DisplayName="猛冲攻击动画")
	TObjectPtr<UPaperFlipbook> FB_DashAttack;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category="Flipbook", DisplayName="死亡动画")
	TObjectPtr<UPaperFlipbook> FB_Death;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category="Flipbook", DisplayName="死亡姿势")
	TObjectPtr<UPaperFlipbook> FB_DeathPose;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category="Flipbook", DisplayName="受伤动画")
	TObjectPtr<UPaperFlipbook> FB_Hurted;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category="Flipbook", DisplayName="滑行动画")
	TObjectPtr<UPaperFlipbook> FB_Slide;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category="Flipbook", DisplayName="攻击动画")
	TObjectPtr<UPaperFlipbook> FB_Attack;
	
	UPROPERTY(Replicated, BlueprintReadWrite, Category="FlipBook", DisplayName="持久动画帧动画状态")
	EFlipBookState FlipBookState=EFlipBookState::Transitioning;
	
	UPROPERTY(Replicated, BlueprintReadWrite, Category="FlipBook", DisplayName="临时动画帧动画状态")
	EFlipBookTemporaryAnimation FlipBookTemporaryAnimationState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement Direction", DisplayName="向前移动方向")
	FVector MoveForwarDirection;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement Direction", DisplayName="向上移动方向")
	FVector MoveUpDirection;

	UPROPERTY(Replicated, BlueprintReadWrite, Category="Movement", DisplayName="上一个移动位置")
	FVector PreCharacterLocation;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category="Movement", DisplayName="跳跃过渡到下落的时间间隔")
	float JumpToFallinginterval = 1.0f;

	UPROPERTY(Replicated, BlueprintReadWrite, Category="Movement", DisplayName="下落时间")
	float FallingTime = 0;

	UPROPERTY(Replicated, BlueprintReadWrite, Category="Camera", DisplayName="摄像机Actor")
	class AUnreal2DCameraActor* CameraActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Climb", DisplayName="攀爬组件")
	class UUreal2DClimbableComponent* ClimbableComponent;

public:
	virtual void BeginDestroy() override;

	virtual void Tick(float DeltaSeconds) override;
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Unreliable, BlueprintCallable, Category="Camera", DisplayName="设置玩家角色摄像机")
	virtual void SetCameraActor(AUnreal2DCameraActor* InCameraActor);

	UFUNCTION(Server, Unreliable, BlueprintCallable, Category="FlipBook", DisplayName="设置玩家角色动画状态")
	void SetFlipBookState(EFlipBookState InFlipBookState);

	UFUNCTION(Server, Unreliable, BlueprintCallable, Category="FlipBook", DisplayName="设置玩家角色动画状态")
	void SC_SetSpriteRotation(FRotator SpriteRotator);
	
	UFUNCTION(NetMulticast, Unreliable, BlueprintCallable, Category="FlipBook", DisplayName="设置玩家角色动画状态")
	void SetSpriteRotation(FRotator SpriteRotator);

protected:
	/*
	 * @description: 横版前移
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月15日 星期五 18:07:17
	 */
	virtual void GroundMovement(const FInputActionValue& ActionValue);

	/*
	 * @description: 蹲伏
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月15日 星期五 18:07:51
	 */
	virtual void CrouchButtomPressed();

	/*
	 * @description: 取消蹲伏
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月15日 星期五 18:07:51
	 */
	virtual void CrouchButtomReleased();

	/*
	 * @description: 攀爬
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月15日 星期五 18:07:30
	 */
	virtual void ClimbUpButtomPressed(const FInputActionValue& ActionValue);
	virtual void ClimbDownButtomPressed(const FInputActionValue& ActionValue);

	/*
	 * @description: 跳跃
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月15日 星期五 18:07:51
	 */
	virtual void JumpButtomPressed();

	/*
	 * @description: 攻击
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月15日 星期五 18:07:51
	 */
	virtual void AttackButtomPressed();

	/*
	 * @description: 猛冲
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月15日 星期五 18:07:51
	 */
	virtual void DashButtomPressed();

	/*
	 * @description: 滑动
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月15日 星期五 18:07:51
	 */
	virtual void SlideButtomPressed();

	/*
	 * @description: 更新玩家角色的各种状态
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月15日 星期五 18:07:51
	 */
	virtual void UpdateFlipBookState(float DeltaTime);

	/*
	 * @description: 根据玩家角色的各种状态来选择对应的 Flipbook，更新玩家角色的动画。
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月15日 星期五 18:07:51
	 */
	virtual void FlipBookStateMachine();

	bool IsCurrentFlipBookAnimationPlayEnd();

	void TemporaryAnimationPlayEndHandle();

public:

	FORCEINLINE UUreal2DClimbableComponent* GetClimbComponent() const { return ClimbableComponent; };
};
