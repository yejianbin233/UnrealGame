// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterAnimInstance.h"
#include "BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "UnrealGame/Weapon/Weapon.h"

void UBlasterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
}

void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (BlasterCharacter == nullptr)
	{
		BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
	}

	if (BlasterCharacter == nullptr)
	{
		return;
	}

	FVector Velocity = BlasterCharacter->GetVelocity();

	Velocity.Z = 0.0f;
	Speed = Velocity.Size();

	// 是否在空中
	bIsAir = BlasterCharacter->GetCharacterMovement()->IsFalling();

	bIsAccelerating = BlasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.0f ? true : false;

	bIsWeaponEquipped = BlasterCharacter->IsWeaponEquipped();
	
	EquippedWeapon = BlasterCharacter->GetEquippedWeapon();
	
	bIsCrouched = BlasterCharacter->bIsCrouched;

	bIsAiming = BlasterCharacter->IsAiming();

	TurningInPlace = BlasterCharacter->GetTurningInPlace();

	bRotateRootBone = BlasterCharacter->ShouldRotateRootBone();

	bElimmed = BlasterCharacter->IsElimmed();
	
	FRotator AimRotation = BlasterCharacter->GetBaseAimRotation();
	
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(BlasterCharacter->GetVelocity());
	
	// UE_LOG(LogTemp, Warning, TEXT("AimRotation Yaw: %f"), AimRotation.Yaw);
	// UE_LOG(LogTemp, Warning, TEXT("MovementRotation Yaw: %f"), MovementRotation.Yaw);
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaSeconds, 15.f);
	
	YawOffset = DeltaRotation.Yaw;

	// UE_LOG(LogTemp, Warning, TEXT("YawOffset: %f"), YawOffset);

	CharacterRotationLastFrame = CharacterRotation;
	
	CharacterRotation = BlasterCharacter->GetActorRotation();

	// 旋转差量 - DeltaRotation
	const FRotator CharacterDeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = CharacterDeltaRotation.Yaw / DeltaSeconds;
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaSeconds, 6.0f);

	// 
	Lean = FMath::Clamp(Interp, -90.0f, 90.0f);

	AO_Yaw = BlasterCharacter->GetAO_Yaw();
	AO_Pitch = BlasterCharacter->GetAO_Pitch();

	// FABR_IK
	if (bIsWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && BlasterCharacter->GetMesh())
	{
		// 获取"枪"模型插槽位置
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket")
			, ERelativeTransformSpace::RTS_World);

		FVector OutPosition;
		FRotator OutRotation;

		// 将世界坐标的 Socket 位置转换成指定骨骼的骨骼空间位置，这样方便指定骨骼转换在其空间中进行转换
		// 注意：转换成 hand_r(右手)骨骼的骨骼空间
		BlasterCharacter->GetMesh()->TransformToBoneSpace(
			FName("hand_r")
			, LeftHandTransform.GetLocation()
			, FRotator::ZeroRotator
			, OutPosition
			, OutRotation);

		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));

		if (BlasterCharacter->IsLocallyControlled())
		{
			// 本地玩家才需要旋转调整枪口的指向
			FTransform RightHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("Hand_R")
			, ERelativeTransformSpace::RTS_World);

			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(
				RightHandTransform.GetLocation()
				, RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - BlasterCharacter->GetHitTarget()));

			// 枪口方向与射击视口前方向之间的旋转值，用于旋转枪头使其对齐视口方向
			RightHandRotation = FMath::RInterpTo(RightHandRotation, LookAtRotation, DeltaSeconds, 30.0f);
		}

		// Debug
		FTransform MuzzleTipTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("MuzzleFlash"), ERelativeTransformSpace::RTS_World);
		FVector MuzzleX(FRotationMatrix(MuzzleTipTransform.GetRotation().Rotator()).GetUnitAxis(EAxis::X));
		DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), MuzzleTipTransform.GetLocation() + MuzzleX * 1000.0f, FColor::Red);
		DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), BlasterCharacter->GetHitTarget(), FColor::Orange);
		// Debug

		// 不在重装弹状态时维持 FABRIK
		bUseFABRIK = BlasterCharacter->GetCombatState() != ECombatState::ECS_Reloading;
	}
}
