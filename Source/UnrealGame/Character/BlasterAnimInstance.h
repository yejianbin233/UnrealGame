// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "UnrealGame/Enums/UnrealGameEnumInterface.h"
#include "BlasterAnimInstance.generated.h"

/**
 * 
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

private:

	UPROPERTY(BlueprintReadOnly, Category = "Character", meta=(AllowPrivateAccess=true))
	class ABlasterCharacter* BlasterCharacter;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta=(AllowPrivateAccess=true))
	float Speed;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta=(AllowPrivateAccess=true))
	bool bIsAir; // 是否在空中

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta=(AllowPrivateAccess=true))
	bool bIsAccelerating; // 是否正在加速

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta=(AllowPrivateAccess=true))
	bool bIsWeaponEquipped;

	class AWeapon* EquippedWeapon;

	UPROPERTY(BlueprintReadOnly, Category= "Movement", meta=(AllowPrivateAccess=true))
	bool bIsCrouched;

	UPROPERTY(BlueprintReadOnly, Category= "Movement", meta=(AllowPrivateAccess=true))
	bool bIsAiming;

	UPROPERTY(BlueprintReadOnly, Category= "Movement", meta=(AllowPrivateAccess=true))
	float YawOffset;

	UPROPERTY(BlueprintReadOnly, Category= "Movement", meta=(AllowPrivateAccess=true))
	float Lean;

	// AO -> AimOffset(瞄准偏移)
	UPROPERTY(BlueprintReadOnly, Category= "Movement", meta=(AllowPrivateAccess=true))
	float AO_Yaw;

	UPROPERTY(BlueprintReadOnly, Category= "Movement", meta=(AllowPrivateAccess=true))
	float AO_Pitch;

	UPROPERTY(BlueprintReadOnly, Category= "Movement", meta=(AllowPrivateAccess=true))
	FTransform LeftHandTransform;

	UPROPERTY(BlueprintReadOnly, Category= "Movement", meta=(AllowPrivateAccess=true))
	ETurningInPlace TurningInPlace;

	FRotator CharacterRotationLastFrame;
	
	FRotator CharacterRotation;

	FRotator DeltaRotation;

	// 旋转枪，使其枪口与射击方向对齐
	UPROPERTY(BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess=true))
	FRotator RightHandRotation;
	
	UPROPERTY(BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess=true))
	bool bRotateRootBone;

	UPROPERTY(BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess=true))
	bool bElimmed;

	UPROPERTY(BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess=true))
	bool bUseFABRIK;
};
