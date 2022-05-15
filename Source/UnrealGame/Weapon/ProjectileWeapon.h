// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnrealGame/Weapon/Weapon.h"
#include "ProjectileWeapon.generated.h"

/**
 * 
 */
UCLASS()
class UNREALGAME_API AProjectileWeapon : public AWeapon
{
	GENERATED_BODY()

public:
	virtual void Fire(const FVector& HitTarget) override;
	
protected:

private:

	// 发射物类
	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	TSubclassOf<class AProjectile> ProjectileClass;

	
	
};
