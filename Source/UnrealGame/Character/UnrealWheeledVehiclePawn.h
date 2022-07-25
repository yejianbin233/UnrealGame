// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "UnrealWheeledVehiclePawn.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class USoundCue;
struct FInputActionValue;

/**
 * 
 */
UCLASS()
class UNREALGAME_API AUnrealWheeledVehiclePawn : public AWheeledVehiclePawn
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Component", DisplayName="摄像机弹簧臂组件")
	USpringArmComponent* SpringArmComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Component", DisplayName="摄像机组件")
	UCameraComponent* CameraComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Component", DisplayName="车辆音频组件")
	UAudioComponent* VehicleAudioComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Sound", DisplayName="车辆引擎声音")
	TObjectPtr<USoundCue> VehicleEngineSoundCue;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="EnhancedInput | InputMappingContext", DisplayName="车辆输入上下文")
	TObjectPtr<UInputMappingContext> VehicleInputMappingContext;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="EnhancedInput | InputAction", DisplayName="车辆向前移动")
	TObjectPtr<UInputAction> EIA_Vehicle_MoveForward;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="EnhancedInput | InputAction", DisplayName="车辆向右移动")
	TObjectPtr<UInputAction> EIA_Vehicle_Right;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="EnhancedInput | InputAction", DisplayName="车辆制动")
	TObjectPtr<UInputAction> EIA_Vehicle_Brake;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="EnhancedInput | InputAction", DisplayName="车辆手动制动")
	TObjectPtr<UInputAction> EIA_Vehicle_HandBrake;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="EnhancedInput | InputAction", DisplayName="车辆取消手动制动")
	TObjectPtr<UInputAction> EIA_Vehicle_CancleHandBrake;

public:
	AUnrealWheeledVehiclePawn();

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void MoveForward(const FInputActionValue& ActionValue);
	void MoveRight(const FInputActionValue& ActionValue);
	void Brake(const FInputActionValue& ActionValue);
	void PressedHandBrake();
	void ReleaseHandBrake();
};


