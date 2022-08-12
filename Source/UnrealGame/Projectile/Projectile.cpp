// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "SimulationPredictProjectile.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "UnrealGame/Character/BlasterCharacter.h"
#include "UnrealGame/Component/LagCompensation/PlayerLagCompensationComponent.h"
#include "UnrealGame/PlayerController/BlasterPlayerController.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = false;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);

	ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
	ProjectileMeshComponent->SetupAttachment(RootComponent);
	ProjectileMeshComponent->AddWorldTransform(DeltaTransform);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;

	BulletTaillParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BulletTaillParticleSystemComponent"));
}

void AProjectile::RealTimeProjectile()
{
	ProjectileHitMode = EProjectileHitMode::RealTime;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	//
	BulletTaillParticleSystemComponent->Activate();

	// 添加子弹发射粒子效果
	if (ProjectileLaunchParticleSystem)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ProjectileLaunchParticleSystem, GetActorTransform());
	}
	
	// 播放子弹射击声音
	if (BulletLaunchSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), BulletLaunchSound, GetActorLocation());
	}

	// 添加击中处理，但只有服务器才能应用伤害处理
	CollisionBox->OnComponentHit.AddDynamic(this, &AProjectile::AProjectile::OnHit);
}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
	FVector NormalImpulse, const FHitResult& HitResult)
{
	
	// TODO 碰撞后销毁，可拓展碰撞指定物理材质才销毁子弹
	Destroy();

	// TODO 击中效果，可拓展与指定物理材质碰撞时对应的击中效果，粒子效果或声音效果等
	// TODO 不应该在销毁时生成粒子效果，可能存在其他消亡条件，其他条件下消亡执行的逻辑应是不一样的
	if (BulletImpactParticleSystem)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletImpactParticleSystem, GetActorTransform());
	}

	if (BulletImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), BulletImpactSound, GetActorLocation());
	}

	if(HasAuthority())
	{
		// 具有服务器网络权限才进行的相关处理
		// 预测子弹不处理 Hit 碰撞
		if (ProjectileHitMode == EProjectileHitMode::RealTime)
		{
			// TODO 应用伤害
		}
	}
}

void AProjectile::Destroyed()
{
	Super::Destroyed();
}

void AProjectile::PredictProjectile(float PredictTime)
{
	// 子弹预测
	FPredictProjectilePathParams PredictProjectilePathParams;
	PredictProjectilePathParams.bTraceWithChannel = true;
	PredictProjectilePathParams.bTraceWithCollision = true;
	PredictProjectilePathParams.DrawDebugTime = 5.f;
	PredictProjectilePathParams.DrawDebugType = EDrawDebugTrace::ForDuration;
	PredictProjectilePathParams.MaxSimTime = 4.0f;
	PredictProjectilePathParams.ProjectileRadius = 5.0f;
	PredictProjectilePathParams.SimFrequency = 30.0f;
	PredictProjectilePathParams.TraceChannel = ECC_Visibility;
	PredictProjectilePathParams.ActorsToIgnore.Add(this);
	PredictProjectilePathParams.StartLocation = GetActorLocation();
	PredictProjectilePathParams.LaunchVelocity = GetActorForwardVector() * ProjectileMovementComponent->InitialSpeed;
	
	FPredictProjectilePathResult PredictProjectilePathResult;
	
	UGameplayStatics::PredictProjectilePath(GetWorld(), PredictProjectilePathParams, PredictProjectilePathResult);

	FName SimulationName = *FString::Printf(TEXT("%s%f"), *this->GetName(), PredictTime);
	if (PredictProjectilePathResult.PathData.Num() > 0)
	{
		for (auto Path : PredictProjectilePathResult.PathData)
		{
			PredictTime += Path.Time;

			if (SimulationPredictProjectileClass)
			{
				// TODO 生成预测子弹
				FActorSpawnParameters SimulationProjectileSpawnParameters;
				SimulationProjectileSpawnParameters.Owner = GetOwner();
				SimulationProjectileSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				ASimulationPredictProjectile* SimulationPredictProjectile = GetWorld()->SpawnActor<ASimulationPredictProjectile>(SimulationPredictProjectileClass, Path.Location, FRotator(0), SimulationProjectileSpawnParameters);
				// TODO 临时预测持续 1 s.
				SimulationPredictProjectile->Simulation(1, CollisionBox->GetScaledBoxExtent());
			}
			
			TArray<AActor*> PlayerControllers;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerController::StaticClass(), PlayerControllers);

			if (PlayerControllers.Num() > 0)
			{
				// 重现的玩家角色数据需要与子弹对应，不与其他子弹碰撞
				for (auto PlayerControllerItem : PlayerControllers)
				{
					ABlasterPlayerController* PlayerController =  Cast<ABlasterPlayerController>(PlayerControllerItem);

					if (PlayerController)
					{
						ABlasterCharacter*  PlayerCharacter = Cast<ABlasterCharacter>(PlayerController->GetPawn());

						if (PlayerCharacter && PlayerCharacter->GetPlayerLagCompensationComponent())
						{
							PlayerCharacter->GetPlayerLagCompensationComponent()->ReapperPlayerCharacterInTimePoint(PredictTime, 0.5f, SimulationName);
						}
					}
				}
			}
		}
	}
}


