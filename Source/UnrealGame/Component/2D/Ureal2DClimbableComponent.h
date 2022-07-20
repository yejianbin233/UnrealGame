// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Components/ActorComponent.h"
#include "UnrealGame/SpriteActor/ClimbableSpriteActor.h"
#include "Ureal2DClimbableComponent.generated.h"


UENUM(BlueprintType)
enum class EFlipBookClimbingState : uint8
{
	Idle UMETA(DisplayName="空闲状态"),
	Climbing UMETA(DisplayName="爬行状态"),
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALGAME_API UUreal2DClimbableComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(Replicated, BlueprintReadWrite, Category="Climb", DisplayName="攀爬类型")
	EClimbableType ClimbingType;

	UPROPERTY(Replicated, BlueprintReadWrite, Category="Climb", DisplayName="攀爬状态")
	EFlipBookClimbingState ClimbingState;

	UPROPERTY(Replicated, BlueprintReadWrite, Category="Climb", DisplayName="是否可攀爬")
	bool bIsCanClimb;

	UPROPERTY(Replicated, BlueprintReadWrite, Category="Climb", DisplayName="当前可攀爬Actor")
	class AClimbableSpriteActor* CurrentClimbableActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Flipbook", DisplayName="边缘攀爬悬挂空闲动画")
	TObjectPtr<class UPaperFlipbook> FB_EdgeIdle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Flipbook", DisplayName="墙边向下滑行动画")
	TObjectPtr<class UPaperFlipbook> FB_WallEdgeSlide;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Flipbook", DisplayName="梯子攀爬动画")
	TObjectPtr<class UPaperFlipbook> FB_LadderClimb;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Flipbook", DisplayName="梯子攀爬姿势")
	TObjectPtr<class UPaperFlipbook> FB_LadderClimbPose;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Flipbook", DisplayName="边缘攀爬抓住动画")
	TObjectPtr<class UPaperFlipbook> FB_EdgeGrab;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputMappingContext")
	TObjectPtr<class UInputMappingContext> Unreal2DClimbInputMappingContext;
	
	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="向左攀爬")
	TObjectPtr<class UInputAction> EIA_MoveLeft;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="向右攀爬")
	TObjectPtr<class UInputAction> EIA_MoveRight;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="向上攀爬")
	TObjectPtr<class UInputAction> EIA_MoveUp;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="向下攀爬")
	TObjectPtr<class UInputAction> EIA_MoveDown;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="向左键松开按压")
	TObjectPtr<class UInputAction> EIA_ReleaseLeft;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="向右键松开按压")
	TObjectPtr<class UInputAction> EIA_ReleaseRight;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="向上键松开按压")
	TObjectPtr<class UInputAction> EIA_ReleaseUp;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="向下键松开按压")
	TObjectPtr<class UInputAction> EIA_ReleaseDown;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="攀爬跳跃")
	TObjectPtr<class UInputAction> EIA_ClimbToJump;
	
private:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category="Climb", DisplayName="攀爬速率", meta=(AllowPrivateAccess))
	float ClimbRate = 1.f;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category="Climb", DisplayName="攀爬速度", meta=(AllowPrivateAccess))
	float ClimbSpeed = 100.f;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category="Climb", DisplayName="X轴方向的跳跃力", meta=(AllowPrivateAccess))
	float XImpulseForceValue = 10000.f;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category="Climb", DisplayName="Z轴方向的跳跃力", meta=(AllowPrivateAccess))
	float ZImpulseForceValue = 10000.f;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category="Climb", DisplayName="第二个方向的原占比", meta=(AllowPrivateAccess))
	float SecondDirectionRatio = 1;

	UPROPERTY()
	// 缓存玩家角色上一次更新位置
	FVector LastUpdatePlayerActorLocation;

	// 按键按下 / 松开
	UPROPERTY()
	bool LeftRelease = true;

	UPROPERTY()
	bool RightRelease = true;

	UPROPERTY()
	bool UpRelease = true;

	UPROPERTY()
	bool DownRelease = true;

public:	
	// Sets default values for this component's properties
	UUreal2DClimbableComponent();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Category="Climb", DisplayName="进入攀爬模式，暂定攀爬模式为 EMovementMode::MOVE_None")
	void EnterClimbMode();

	UFUNCTION(Category="Climb", DisplayName="结束攀爬模式，正常模式为 EMovementMode::MOVE_Walk")
	void EndClimbMode();

	UFUNCTION(Category="Climb", DisplayName="进入攀爬状态")
	void EnterClimbState();

	UFUNCTION(Category="Climb", DisplayName="结束攀爬状态")
	void EndClimbState();

	UFUNCTION(Category="Climb", DisplayName="进入或离开可攀爬区域")
	void EnterOrLeaveClimbableArea(AClimbableSpriteActor* ClimbableSpriteActor, bool IsEnter);

	UFUNCTION(Category="Climb", DisplayName="设置可攀爬")
	void ClimbableHandle(bool ToUp);

	/*
	 * @description: FlipBookClimbingStateMachine - 攀爬状态下的状态机处理逻辑
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月20日 星期三 15:07:50
	 */
	void FlipBookClimbingStateMachine();

	// 获取玩家角色
	class AUnreal2DCharacter* GetOwnerPlayerCharacter();
	
	/*
	 * @description: 攀爬输入
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月15日 星期五 18:07:30
	 */
	void ClimbLeftMovement(const FInputActionValue& ActionValue);
	
	void ClimbRightMovement(const FInputActionValue& ActionValue);

	void ClimbUpMovement(const FInputActionValue& ActionValue);
	
	void ClimbDownMovement(const FInputActionValue& ActionValue);
	
	void ClimbToJump();
	void ApplyClibToJump(FVector ImpulseForce);
	void ReleaseLeft();
	void ReleaseRight();
	void ReleaseUp();
	void ReleaseDown();
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 绑定攀爬用户输入
	void InitPlayerInputComponent();
};
