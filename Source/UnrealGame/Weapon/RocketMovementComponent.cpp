// Fill out your copyright notice in the Description page of Project Settings.


#include "RocketMovementComponent.h"

UProjectileMovementComponent::EHandleBlockingHitResult URocketMovementComponent::HandleBlockingHit(
	const FHitResult& Hit, float TimeTick, const FVector& MoveDelta, float& SubTickTimeRemaining)
{
	EHandleBlockingHitResult HandleBlockingHitResult = Super::HandleBlockingHit(Hit, TimeTick, MoveDelta, SubTickTimeRemaining);

	// 阻塞时仍继续模拟运动
	return EHandleBlockingHitResult::AdvanceNextSubstep;
}

void URocketMovementComponent::HandleImpact(const FHitResult& Hit, float TimeSlice, const FVector& MoveDelta)
{
	Super::HandleImpact(Hit, TimeSlice, MoveDelta);
}
