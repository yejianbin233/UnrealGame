// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LongRangeWeapon.h"
#include "ProjectileWeapon.generated.h"

/**
 * 
 */
UCLASS()
class UNREALGAME_API AProjectileWeapon : public ALongRangeWeapon
{
	GENERATED_BODY()

public:
	
protected:

private:

	// 发射物类
	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	TSubclassOf<class AProjectile> ProjectileClass;

	
	
};
