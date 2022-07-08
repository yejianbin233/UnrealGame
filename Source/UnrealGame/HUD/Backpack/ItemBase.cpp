// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemBase.h"

#include "Components/SphereComponent.h"
#include "UnrealGame/Component/PickableComponent.h"

// Sets default values
AItemBase::AItemBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));

	SkeletalMeshComponent->SetupAttachment(RootComponent);
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
}

void AItemBase::InitMesh(FItemInfo ItemInfo)
{
	switch (ItemInfo.MeshType)
	{
		case EMeshType::SkeletalMesh:
			{
				// USkeletalMeshComponent* SkeletalMeshComponent = NewObject<USkeletalMeshComponent>(this, TEXT("SkeletalMeshComponent"));
				//
				// //重要，否则无法在细节面板中看到组件
				// this->AddInstanceComponent(SkeletalMeshComponent);
				//
				// //重要，必须要注册组件
				// SkeletalMeshComponent->RegisterComponent();

				//组件绑定，使组件附加到跟组件(即 Actor)上，与在构造函数中调用的不同
				// SkeletalMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				SkeletalMeshComponent->SetupAttachment(RootComponent, TEXT("SkeletalMeshComponent"));
				
				SkeletalMeshComponent->SetSkeletalMesh(ItemInfo.SkeletalMesh);

				SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				SkeletalMeshComponent->SetCollisionObjectType(CollisionObjectType);
				SkeletalMeshComponent->SetCollisionProfileName(CollisionProfileName);

				// TODO 模拟物理
				// SkeletalMeshComponent->SetSimulatePhysics(true);
				// SkeletalMeshComponent->SetEnableGravity(true);

				RemoveOwnedComponent(StaticMeshComponent);
				
				break;
			}
		case EMeshType::StaticMesh:
			{
				// UStaticMeshComponent* StaticMeshComponent = NewObject<UStaticMeshComponent>(this, TEXT("SkeletalMeshComponent"));
				// //重要，否则无法在细节面板中看到组件
				// this->AddInstanceComponent(StaticMeshComponent);
				//
				// //重要，必须要注册组件
				// StaticMeshComponent->RegisterComponent();

				//组件绑定，使组件附加到跟组件(即 Actor)上，与在构造函数中调用的不同
				// StaticMeshComponent->AttachToComponent(RootComponent,FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				StaticMeshComponent->SetupAttachment(RootComponent, TEXT("StaticMeshComponent"));
				
				StaticMeshComponent->SetStaticMesh(ItemInfo.StaticMesh);

				StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				StaticMeshComponent->SetCollisionObjectType(CollisionObjectType);
				StaticMeshComponent->SetCollisionProfileName(CollisionProfileName);

				// TODO 模拟物理
				// StaticMeshComponent->SetSimulatePhysics(true);
				// StaticMeshComponent->SetEnableGravity(true);

				RemoveOwnedComponent(SkeletalMeshComponent);
				
				break;
			}
	}
	
	PickableComponent = NewObject<UPickableComponent>(this, TEXT("PickableComponent"));;
	this->AddInstanceComponent(StaticMeshComponent);
	
	//重要，必须要注册组件
	StaticMeshComponent->RegisterComponent();
	
	PickableComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	PickableComponent->PickableAreaComponent->AttachToComponent(PickableComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	PickableComponent->UpdatePickupState(PickableComponent->PickupState);
}

// Called when the game starts or when spawned
void AItemBase::BeginPlay()
{
	Super::BeginPlay();
	
}

