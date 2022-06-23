// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnrealGame/Weapon/Projectile.h"
#include "ProjectileBullet.generated.h"

/**
 * 
 */
UCLASS()
class UNREALGAME_API AProjectileBullet : public AProjectile
{
	GENERATED_BODY()


public:

	AProjectileBullet();
	
protected:

	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& HitResult) override;
	
private:
};
