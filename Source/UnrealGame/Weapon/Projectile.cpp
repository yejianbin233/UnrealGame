// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealGame/Weapon/Projectile.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "UnrealGame/UnrealGame.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);

	ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
	ProjectileMeshComponent->SetupAttachment(RootComponent);
	ProjectileMeshComponent->AddWorldTransform(DeltaTransform);
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

	// TODO 击中处理
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

