// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBullet.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AProjectileBullet::AProjectileBullet()
{
	
}

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& HitResult)
{
	// 在生成 Projectile 类型时，设置了 Owner
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (OwnerCharacter)
	{
		AController* OwnerController = OwnerCharacter->Controller;

		if (OwnerController)
		{
			// 应用伤害事件，如果需要得到处理，应绑定伤害事件委托
			UGameplayStatics::ApplyDamage(OtherActor
					, Damage
					, OwnerController
					, this
					, UDamageType::StaticClass()
				);
	
			Super::OnHit(HitComponent, OtherActor, OtherComponent, NormalImpulse, HitResult);
		}
	}
	
}
