// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"
#include "Components/TimelineComponent.h"
#include "UnrealGame/Enums/UnrealGameEnumInterface.h"
#include "UnrealCameraComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(CameraLog, Log, All);

UCLASS(BlueprintType, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALGAME_API UUnrealCameraComponent : public USceneComponent
{
	GENERATED_BODY()

public:

	// PlayerInput
	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputMappingContext")
	TObjectPtr<class UInputMappingContext> CameraModeInputMappingContext;
	
	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="截图")
	TObjectPtr<class UInputAction> EIA_Shot;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="放大")
	TObjectPtr<class UInputAction> EIA_Zoom;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="关闭摄像机")
	TObjectPtr<class UInputAction> EIA_CloseCamera;

	// 需要通过调用 ABlasterPlayerController 的 ConsoleSwitchInputMappingContext 控制台可执行函数来切换到摄像机输入模式，然后通过指定按键进入摄像机调试模式
	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", SimpleDisplay, DisplayName="进入摄像机调试模式")
	TObjectPtr<class UInputAction> EIA_EnterDebugMode;
	
	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", SimpleDisplay, DisplayName="退出摄像机调试模式")
	TObjectPtr<class UInputAction> EIA_LeaveDebugMode;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", SimpleDisplay, DisplayName="调整摄像机弹簧臂")
	TObjectPtr<class UInputAction> EIA_AdjustArmLength;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", SimpleDisplay, DisplayName="选择视点(1)")
	TObjectPtr<class UInputAction> EIA_SelectViewPoint1;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", SimpleDisplay, DisplayName="选择视点(2)")
	TObjectPtr<class UInputAction> EIA_SelectViewPoint2;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", SimpleDisplay, DisplayName="选择视点(3)")
	TObjectPtr<class UInputAction> EIA_SelectViewPoint3;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", SimpleDisplay, DisplayName="选择视点(4)")
	TObjectPtr<class UInputAction> EIA_SelectViewPoint4;
	
	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", SimpleDisplay, DisplayName="重置")
	TObjectPtr<class UInputAction> EIA_Reset;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", SimpleDisplay, DisplayName="X轴目标偏移")
	TObjectPtr<class UInputAction> EIA_TargetOffsetX;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", SimpleDisplay, DisplayName="Y轴目标偏移")
	TObjectPtr<class UInputAction> EIA_TargetOffsetY;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", SimpleDisplay, DisplayName="Z轴目标偏移")
	TObjectPtr<class UInputAction> EIA_TargetOffsetZ;
	// ===============

	// Reference

	UPROPERTY(VisibleAnywhere, Category="Reference", DisplayName="玩家角色")
	class ABlasterCharacter* PlayerCharacter;

	UPROPERTY(VisibleAnywhere, Category="Reference", DisplayName="玩家控制器")
	class APlayerController* PlayerController;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera", DisplayName="摄像机弹簧臂组件")
	class USpringArmComponent* SpringArmComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera", DisplayName="摄像机组件")
	class UCameraComponent* CameraComponent;

	UPROPERTY(EditAnywhere, Category="Camera Widget", DisplayName="摄像机控件类")
	TSubclassOf<class UCameraWidget> CameraWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category="Camera Widget", DisplayName="摄像机用户控件")
	class UCameraWidget* CameraViewWidget;

	// Camera Properties
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Camera", DisplayName="第一人称摄像机弹簧臂默认长度")
	float DefaultFirstTargetArmLength;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Camera", DisplayName="第三人称摄像机弹簧臂默认长度")
	float DefaultThreeTargetArmLength;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Debug", SimpleDisplay, DisplayName="调试模式右视口摄像机弹簧臂默认长度")
	float DefaultRightViewPointTargetArmLength;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Debug", SimpleDisplay, DisplayName="调试模式顶部视口摄像机弹簧臂默认长度")
	float DefaultTopViewPointTargetArmLength;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Debug", SimpleDisplay, DisplayName="弹簧臂长度调整速度")
	float ArmLengthSpeed = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Camera", DisplayName="弹簧臂X轴目标偏移速度")
	float ArmTartOffsetXSpeed = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Camera", DisplayName="弹簧臂Y轴目标偏移速度")
	float ArmTartOffsetYSpeed = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Camera", DisplayName="弹簧臂Z轴目标偏移速度")
	float ArmTartOffsetZSpeed = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Camera", DisplayName="第一人称视口骨骼位置插槽")
	FName FirstPersonSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Camera", DisplayName="第一人称视口骨骼位置插槽")
	FName ThreePersonSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Camera", DisplayName="右视角旋转值")
	FRotator RightDebugViewPointRotator = FRotator(0,-90,0);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Camera", DisplayName="顶部视角旋转值")
	FRotator TopDebugViewPointRotator = FRotator(-90,0,0);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Debug", DisplayName="调试箭头大小")
	float ArrowSize=30.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Debug", DisplayName="绘制球体半径")
	float DebugSpereRadius=30.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Debug", DisplayName="绘制球体分段")
	float DebugSpereSegment=30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera", DisplayName="摄像机视点(第一人称/第三人称)")
	ECameraViewPointMode ViewPointMode;

	UPROPERTY(EditDefaultsOnly, Category="Interp Curve", DisplayName="第一人称->第三人称插值曲线资产")
	UCurveFloat* FirstToThreeInterpolationCurve;

	UPROPERTY(EditDefaultsOnly, Category="Shot Flash Curve", DisplayName="摄像闪光曲线资产")
	UCurveFloat* ShotFlashCurve;

	// 第一人称 -> 第三人称视角过渡时间轴
	FTimeline FirstToThreeInterpolationTimeline;
	// 第一人称 -> 第三人称视角过渡委托
	FOnTimelineFloat OnStartFirstToThreeInterpolation;

	// 闪光灯时间轴
	FTimeline FlashTimeline;
	FOnTimelineFloat OnShotFlash;

	// 截图
	UPROPERTY(VisibleAnywhere, Category="Camera View", DisplayName="摄像机视口数组")
	TArray<ECameraViewPointMode> CameraViewPointModes;

	UPROPERTY(EditDefaultsOnly, Category="Camera", DisplayName="摄像控制台命令")
	FString ShotConsoleCommand;
	
	UPROPERTY(EditDefaultsOnly, Category="Camera", DisplayName="高清摄像控制台命令")
	FString HighResShotConsoleCommand;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera", DisplayName="截图模式")
	ECameraShotMode CameraShotMode;

private:

	UPROPERTY(BlueprintReadOnly, Category="EnhancedInput", DisplayName="是否已绑定输入", meta=(AllowPrivateAccess))
	bool bIsBindInput;

	UPROPERTY(BlueprintReadOnly, Category="EnhancedInput", DisplayName="是否已打开摄像机", meta=(AllowPrivateAccess))
	bool bIsOpenCamera;

	UPROPERTY(EditDefaultsOnly, Category="FOV", DisplayName="最小视野(FOV)", meta=(UIMin=0))
	float MinFOV;
	UPROPERTY(EditDefaultsOnly, Category="FOV", DisplayName="最大视野(FOV)", meta=(UIMin=0))
	float MaxFOV;

	UPROPERTY(EditDefaultsOnly, Category="Zoom", DisplayName="放大速度", meta=(UIMin=0))
	float ZoomSpeed=1.0f;

	UPROPERTY(VisibleAnywhere, Category="Debug", SimpleDisplay, DisplayName="是否在正在调试摄像机视口")
	bool bIsDebuging;

	UPROPERTY(EditAnywhere, Category="Debug", SimpleDisplay, DisplayName="是否显示摄像机调试绘制信息")
	bool bIsShowCameraDraw=false;

public:
	// Sets default values for this component's properties
	UUnrealCameraComponent();

	UFUNCTION(BlueprintCallable, Category="Camera", DisplayName="设置视点")
	void SetCameraViewPoint(ECameraViewPointMode InViewPointMode);
	
	UFUNCTION(Category="Camera", DisplayName="打开摄像机")
	void OpenCamera();

	UFUNCTION(Category="Camera", DisplayName="关闭摄像机")
	void CloseCamera();

	UFUNCTION(Category="Camera", DisplayName="摄像")
	void Shot();

	UFUNCTION(Category="Camera", DisplayName="自定义实现摄像(非控制台命令)")
	void CustomShot();

	UFUNCTION(Category="Camera", DisplayName="放大")
	void ZoomFOV(const FInputActionValue& ActionValue);

	UFUNCTION(BlueprintCallable, Category="Save", DisplayName="创建摄像贴图")
	static void CreateShotTexture();
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	UFUNCTION(Category="Camera View", DisplayName="切换到第一人称视角")
	virtual void SwitchFirstPersonViewpoint();

	UFUNCTION(Category="Camera View", DisplayName="切换到第三人称视角")
	virtual void SwitchThreePersonViewpoint();

	UFUNCTION(Category="Debug", DisplayName="切换到调试模式右视角")
	virtual void SwitchDebugRightViewpoint();

	UFUNCTION(Category="Debug", DisplayName="切换到调试模式顶部视角")
	virtual void SwitchDebugTopViewPoint();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(Category="Camera Input", DisplayName="绑定摄像机输入")
	virtual void BindCameraInput();

private:

	UFUNCTION(Category="Interpolation", DisplayName="第一人称->第三人称插值")
	void FirstToThreeInterpolationHandle(float InterpolationValue);

	UFUNCTION(Category="Camera Shot", DisplayName="摄像闪光")
	void ShotFlashHandle(float Value);

	// 保存摄像图片
	void SaveScreenShot(FViewport* InViewport);

	UFUNCTION(Category="Debug", DisplayName="进入调试模式")
	void EnterDebugModePressed();

	UFUNCTION(Category="Debug", DisplayName="离开调试模式")
	void LeaveDebugModeRelease();

	UFUNCTION(BlueprintCallable, Category="Debug", DisplayName="选择视点(1)")
	void SelectViewPoint1Pressed();

	UFUNCTION(BlueprintCallable, Category="Debug", DisplayName="选择视点(2)")
	void SelectViewPoint2Pressed();

	UFUNCTION(BlueprintCallable, Category="Debug", DisplayName="选择视点(3)")
	void SelectViewPoint3Pressed();

	UFUNCTION(BlueprintCallable, Category="Debug", DisplayName="选择视点(4)")
	void SelectViewPoint4Pressed();

	UFUNCTION(BlueprintCallable, Category="Debug", DisplayName="重置视点")
	void ResetPressed();

	UFUNCTION(BlueprintCallable, Category="Debug", DisplayName="调整弹簧臂长度")
	void AdjustArmLength(const FInputActionValue& ActionValue);

	UFUNCTION(BlueprintCallable, Category="Debug", DisplayName="调整弹簧臂目标X偏移量")
	void AdjustXOffset(const FInputActionValue& ActionValue);

	UFUNCTION(BlueprintCallable, Category="Debug", DisplayName="调整弹簧臂目标Y偏移量")
	void AdjustYOffset(const FInputActionValue& ActionValue);

	UFUNCTION(BlueprintCallable, Category="Debug", DisplayName="调整弹簧臂目标Z偏移量")
	void AdjustZOffset(const FInputActionValue& ActionValue);
	
public:	
	// Called every frame

	FORCEINLINE UCameraComponent* GetCameraComponent() const { return CameraComponent; }
	
	FORCEINLINE USpringArmComponent* GetSpringArmComponent() const { return SpringArmComponent; }
};
