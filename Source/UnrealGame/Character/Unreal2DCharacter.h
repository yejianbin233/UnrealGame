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
enum class EMovementState : uint8
{
	Idle UMETA(DisplayName="空闲状态"),
	GroundMovement UMETA(DisplayName="地面运动状态"),
	Jumping UMETA(DisplayName="跳跃状态"),
	Falling UMETA(DisplayName="下落状态"),
	Climbing UMETA(DisplayName="攀爬状态")
};

UCLASS()
class UNREALGAME_API AUnreal2DCharacter : public APaperCharacter
{
	GENERATED_BODY()

public:

	AUnreal2DCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Component", DisplayName="弹簧臂组件")
	//class USpringArmComponent* SpringArmComponent;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Component", DisplayName="摄像机组件")
	//class UCameraComponent* CameraComponent;

	/* Enhanced Input  */
	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputMappingContext")
	TObjectPtr<class UInputMappingContext> Unreal2DGameInputMappingContext;
	
	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="向前移动")
	TObjectPtr<UInputAction> EIA_MoveForward;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="攀爬移动")
	TObjectPtr<UInputAction> EIA_Climp;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="跳跃")
	TObjectPtr<UInputAction> EIA_Jump;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category="Flipbook", DisplayName="空闲动画")
	TObjectPtr<UPaperFlipbook> FB_Idle;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category="Flipbook", DisplayName="奔跑动画")
	TObjectPtr<UPaperFlipbook> FB_Run;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category="Flipbook", DisplayName="攀爬动画")
	TObjectPtr<UPaperFlipbook> FB_Climb;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category="Flipbook", DisplayName="跳跃动画")
	TObjectPtr<UPaperFlipbook> FB_Jump;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category="Flipbook", DisplayName="下落动画")
	TObjectPtr<UPaperFlipbook> FB_Falling;
	
	UPROPERTY(Replicated, BlueprintReadWrite, Category="Movement", DisplayName="移动状态")
	EMovementState MovementState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement Direction", DisplayName="向前移动方向")
	FVector MoveForwarDirection;

	UPROPERTY(Replicated, BlueprintReadWrite, Category="Movement", DisplayName="上一个移动位置")
	FVector PreCharacterLocation;

	UPROPERTY(Replicated, BlueprintReadWrite, Category="Movement", DisplayName="上一个移动位置")
	float JumpToFallinginterval = 1.0f;

	UPROPERTY(BlueprintReadWrite, Category="Movement", DisplayName="下落时间")
	float FallingTime = 0;

	UPROPERTY(BlueprintReadWrite, Category="Camera", DisplayName="摄像机Actor")
	class AUnreal2DCameraActor* CameraActor;

public:

	virtual void Tick(float DeltaSeconds) override;
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category="Camera", DisplayName="设置玩家角色摄像机")
	virtual void SetCameraActor(AUnreal2DCameraActor* InCameraActor);

protected:
	/*
	 * @description: 横版前移
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月15日 星期五 18:07:17
	 */
	virtual void MoveForward(const FInputActionValue& ActionValue);

	/*
	 * @description: 攀爬
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月15日 星期五 18:07:30
	 */
	virtual void Climb(const FInputActionValue& ActionValue);

	/*
	 * @description: 跳跃
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月15日 星期五 18:07:51
	 */
	virtual void JumpButtomPressed();

	/*
	 * @description: 更新玩家角色的各种状态
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月15日 星期五 18:07:51
	 */
	virtual void UpdateMovementState(float DeltaTime);

	/*
	 * @description: 根据玩家角色的各种状态来选择对应的 Flipbook，更新玩家角色的动画。
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月15日 星期五 18:07:51
	 */
	virtual void MovementStateMachine();
	
};
