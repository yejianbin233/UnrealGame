// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileRocket.h"

#include "RocketMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AProjectileRocket::AProjectileRocket()
{
	RocketMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RocketMesh"));

	RocketMesh->SetupAttachment(RootComponent);

	// 开启碰撞
	RocketMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RocketMovementComponent = CreateDefaultSubobject<URocketMovementComponent>(TEXT("RocketMovementComponent"));
	RocketMovementComponent->bRotationFollowsVelocity = true;
	RocketMovementComponent->SetIsReplicated(true);
	
}

void AProjectileRocket::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& HitResult)
{

	APawn* FiringPawn = GetInstigator();
	if (FiringPawn)
	{
		AController* FiringController = FiringPawn->GetController();

		if (FiringController)
		{
			// 应用径向衰减伤害
			UGameplayStatics::ApplyRadialDamageWithFalloff(
					this
					, Damage
					, 10.0f
					, GetActorLocation()
					, 200.0f
					, 500.0f
					, 1.0f
					, UDamageType::StaticClass()
					, TArray<AActor*>()
					, this
					, FiringController
				);
		}
	}
	
	Super::OnHit(HitComponent, OtherActor, OtherComponent, NormalImpulse, HitResult);

	
}
