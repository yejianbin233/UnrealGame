// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "RocketMovementComponent.generated.h"


/*
 * @name: URocketMovementComponent - 火箭弹移动组件
 * @description: 自定义火箭弹移动组件
 * 
 * @author: yejianbin
 * @createTime: 2022年05月17日 星期二 19:05:50
 */
/**
 * 
 */
UCLASS()
class UNREALGAME_API URocketMovementComponent : public UProjectileMovementComponent
{
	GENERATED_BODY()

/*
 * Override Function
 */
public:

	
protected:

	virtual EHandleBlockingHitResult HandleBlockingHit(const FHitResult& Hit, float TimeTick, const FVector& MoveDelta, float& SubTickTimeRemaining) override;

	virtual void HandleImpact(const FHitResult& Hit, float TimeSlice, const FVector& MoveDelta) override;
	
private:

	
/* Override End */

	/*
* Custom Variable
*/
public:

	
protected:

	
private:

	
/* Custom Variable End*/


/*
 * Custom Method
 */
public:

	
protected:

	
private:

	
/* Custom Method */

/*
 * Getter / Setter
 */

public:


	
/* Getter / Setter End */
	
};
