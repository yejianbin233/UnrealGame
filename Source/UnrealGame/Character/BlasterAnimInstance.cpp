// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterAnimInstance.h"
#include "BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "UnrealGame/Component/Combat/CombatComponent.h"
#include "UnrealGame/Weapon/Weapon.h"

void UBlasterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	InitialReferences();
}

void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (BlasterCharacter == nullptr)
	{
		BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());

		if (BlasterCharacter == nullptr)
		{
			return;
		}
	}

	UpdateProperties();
	UpdateState();
}

void UBlasterAnimInstance::InitialReferences()
{
	BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
}

void UBlasterAnimInstance::UpdateProperties()
{
	MovementDirection = BlasterCharacter->GetMovementDirection();
	
	Speed = BlasterCharacter->GetVelocity().Size();

	MovementSpeedLevel = BlasterCharacter->GetMovementSpeedLevel();

	AO_YawOffset = BlasterCharacter->GetAO_YawOffset();
	
	AO_PitchOffset = BlasterCharacter->GetAO_PitchOffset();
	
	AO_Blend = BlasterCharacter->GetAO_Blend();
}

void UBlasterAnimInstance::UpdateState()
{
	PlayerEquipState = BlasterCharacter->GetCombatComponent()->GetPlayerEquipState();

	bIsInAir = BlasterCharacter->IsInAir();

	bIsJumping = BlasterCharacter->IsJumping();

	bIsAccelerating = BlasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.0f ? true : false;

	bIsCrouched = BlasterCharacter->bIsCrouched;

	bIsAiming = BlasterCharacter->IsAiming();

	bElimmed = BlasterCharacter->IsElimmed();
}
