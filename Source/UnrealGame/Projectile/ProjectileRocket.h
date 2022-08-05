// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileRocket.generated.h"

class URocketMovementComponent;
/*
 * @name: AProjectileRocket - 火箭子弹类
 * @description: 火箭子弹
 * 
 * @author: yejianbin
 * @createTime: 2022年05月17日 星期二 14:05:39
 */
UCLASS()
class UNREALGAME_API AProjectileRocket : public AProjectile
{
	   GENERATED_BODY()

 
    AProjectileRocket();

/*
 * Override Function
 */
public:

 
protected:

    virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& HitResult) override;
    
 
private:
    
/* Override End */

/*
* Custom Variable
*/
public:

 
protected:

	UPROPERTY(EditAnywhere)
    URocketMovementComponent* RocketMovementComponent;
 
private:

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* RocketMesh;
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
