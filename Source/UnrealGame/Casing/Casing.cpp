// Fill out your copyright notice in the Description page of Project Settings.


#include "Casing.h"

#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
ACasing::ACasing()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CasingMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CasingMeshComponent"));
	SetRootComponent(CasingMeshComponent);

	CasingMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	// 物理模拟
	CasingMeshComponent->SetSimulatePhysics(true);

	// 添加重力
	CasingMeshComponent->SetEnableGravity(true);

	// 通知刚体碰撞
	CasingMeshComponent->SetNotifyRigidBodyCollision(true);

	ShellEjectionImpulse = 10.0f;
}

// Called when the game starts or when spawned
void ACasing::BeginPlay()
{
	Super::BeginPlay();

	// 添加脉冲力
	CasingMeshComponent->AddImpulse(GetActorForwardVector() * ShellEjectionImpulse);
	CasingMeshComponent->OnComponentHit.AddDynamic(this, &ACasing::ACasing::OnHit);
}

void ACasing::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
	FVector NormalImpulse, const FHitResult& HitResult)
{
	if (ShellSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShellSound, GetActorLocation());
	}
	
	Destroy();
}


