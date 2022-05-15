// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealGame/Weapon/Projectile.h"

#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "UnrealGame/UnrealGame.h"
#include "UnrealGame/Character/BlasterCharacter.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);

	// 设置对象的碰撞对象通道，决定与其他碰撞的交互
	CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	// 碰撞启用
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// 统一设置为"Ignore"忽略碰撞响应，然后再针对某些通道进行响应。
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	// 响应通道
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECollisionResponse::ECR_Block);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (Tracer)
	{
		TracerComponent = UGameplayStatics::SpawnEmitterAttached(
			Tracer
			, CollisionBox
			, FName("")
			, GetActorLocation()
			, GetActorRotation()
			, EAttachLocation::KeepWorldPosition
		);
	}

	if (HasAuthority())
	{
		// 绑定命中委托
		CollisionBox->OnComponentHit.AddDynamic(this, &AProjectile::AProjectile::OnHit);
	}
}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
	FVector NormalImpulse, const FHitResult& HitResult)
{
	
	// 碰撞后销毁
	Destroy();
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::Destroyed()
{
	Super::Destroyed();


	// TODO 不应该在销毁时生成粒子效果，可能存在其他消亡条件，其他条件下消亡执行的逻辑应是不一样的
	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, GetActorTransform());
	}

	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, GetActorLocation());
	}

}

