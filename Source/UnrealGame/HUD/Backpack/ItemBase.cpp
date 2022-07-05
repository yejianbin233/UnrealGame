// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemBase.h"

#include "UnrealGame/Component/PickableComponent.h"

// Sets default values
AItemBase::AItemBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	PickableComponent = CreateDefaultSubobject<UPickableComponent>(TEXT("PickableComponent"));

	PickableComponent->SetupAttachment(RootComponent);

}

void AItemBase::InitMesh(FItemInfo ItemInfo)
{
	switch (ItemInfo.MeshType)
	{
		case EMeshType::SkeletalMesh:
			{
				USkeletalMeshComponent* SkeletalMeshComponent = NewObject<USkeletalMeshComponent>(this, TEXT("SkeletalMeshComponent"));

				//重要，否则无法在细节面板中看到组件
				this->AddInstanceComponent(SkeletalMeshComponent);

				//重要，必须要注册组件
				SkeletalMeshComponent->RegisterComponent();

				//组件绑定，使组件附加到跟组件(即 Actor)上，与在构造函数中调用的不同
				SkeletalMeshComponent->AttachToComponent(RootComponent,FAttachmentTransformRules::SnapToTargetNotIncludingScale);

				SkeletalMeshComponent->SetSkeletalMesh(ItemInfo.SkeletalMesh);

				SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				SkeletalMeshComponent->SetCollisionObjectType(CollisionObjectType);
				SkeletalMeshComponent->SetCollisionProfileName(CollisionProfileName);
				break;
			}
		case EMeshType::StaticMesh:
			{
				UStaticMeshComponent* StaticMeshComponent = NewObject<UStaticMeshComponent>(this, TEXT("SkeletalMeshComponent"));
				//重要，否则无法在细节面板中看到组件
				this->AddInstanceComponent(StaticMeshComponent);

				//重要，必须要注册组件
				StaticMeshComponent->RegisterComponent();

				//组件绑定，使组件附加到跟组件(即 Actor)上，与在构造函数中调用的不同
				StaticMeshComponent->AttachToComponent(RootComponent,FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				
				StaticMeshComponent->SetStaticMesh(ItemInfo.StaticMesh);

				StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				StaticMeshComponent->SetCollisionObjectType(CollisionObjectType);
				StaticMeshComponent->SetCollisionProfileName(CollisionProfileName);
				break;
			}
	}
}

// Called when the game starts or when spawned
void AItemBase::BeginPlay()
{
	Super::BeginPlay();
	
}

