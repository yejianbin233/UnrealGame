// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "HitScanWeapon.generated.h"


class UParticleSystem;
/*
 * @name: AHitScanWeapon - 扫射武器
 * @description: 实现扫射攻击的武器
 * 
 * @author: yejianbin
 * @createTime: 2022年05月17日 星期二 19:05:34
 */
/**
 * 
 */
UCLASS()
class UNREALGAME_API AHitScanWeapon : public AWeapon
{
	GENERATED_BODY()


	/*
 * Override Function
 */
public:

	virtual void Fire(const FVector& HitTarget) override;
	
protected:

	
private:

	
	/* Override End */

	/*
* Custom Variable
*/
public:
protected:
private:

	UPROPERTY(EditAnywhere)
	float Damage = 20.0f;

	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactParticleSystem;
	
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
