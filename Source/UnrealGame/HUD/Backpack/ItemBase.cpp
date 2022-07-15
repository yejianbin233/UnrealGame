// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemBase.h"

#include "Components/SphereComponent.h"
#include "UnrealGame/Character/BlasterCharacter.h"

// Sets default values
AItemBase::AItemBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// 网络复制 Actor
	bReplicates = true;

	// 在构造函数中构造物品的基础框架
	
	PhysicMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PhysicMeshComponent"));

	SetRootComponent(PhysicMeshComponent);

	PickableAreaComponent = CreateDefaultSubobject<USphereComponent>(TEXT("PickableAreaComponent"));
	PickableAreaComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	PickableAreaComponent->OnComponentBeginOverlap.AddDynamic(this, &AItemBase::OnComponentBeginOverlap);
	PickableAreaComponent->OnComponentEndOverlap.AddDynamic(this, &AItemBase::OnComponentEndOverlap);
	
}

void AItemBase::Init(FItemInfo ItemInfo)
{
	switch (ItemInfo.MeshType)
	{
		case EMeshType::SkeletalMesh:
			{
				USkeletalMeshComponent* SkeletalMeshComponent = NewObject<USkeletalMeshComponent>(this, TEXT("物品显示网格体"));
				
				// 重要，否则无法在细节面板中看到组件
				this->AddInstanceComponent(SkeletalMeshComponent);
				
				// 重要，必须要注册组件
				SkeletalMeshComponent->RegisterComponent();

				// 组件绑定，使组件附加到跟组件(即 Actor)上，与在构造函数中调用的不同
				SkeletalMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				SkeletalMeshComponent->AddWorldTransform(DeltaTransform);
				
				SkeletalMeshComponent->SetSkeletalMesh(ItemInfo.SkeletalMesh);
				break;
			}
		case EMeshType::StaticMesh:
			{
				UStaticMeshComponent* StaticMeshComponent = NewObject<UStaticMeshComponent>(this, TEXT("物品显示网格体"));

				// 重要，否则无法在细节面板中看到组件
				this->AddInstanceComponent(StaticMeshComponent);
				
				// 重要，必须要注册组件
				StaticMeshComponent->RegisterComponent();

				// 组件绑定，使组件附加到跟组件(即 Actor)上，与在构造函数中调用的不同
				StaticMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				StaticMeshComponent->AddWorldTransform(DeltaTransform);
				
				StaticMeshComponent->SetStaticMesh(ItemInfo.StaticMesh);
				break;
			}
	}

	bIsInitialized = true;
}

// Called when the game starts or when spawned
void AItemBase::BeginPlay()
{
	Super::BeginPlay();

	// 在场景中的物品不会调用 Init，Init 函数是在蓝图类的构造函数中调用的，因此在场景中的物品的都是已经完成初始化的，默认设置为 true。
	bIsInitialized = true;
}

void AItemBase::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority())
	{
		if (bIsInitialized)
		{
			ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
			if (ensure(BlasterCharacter))
			{
				BlasterCharacter->bHasPickableObject = true;
			}
		}
	}
}

void AItemBase::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (HasAuthority())
	{
		if (bIsInitialized)
		{
			ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
			if (ensure(BlasterCharacter))
			{
				BlasterCharacter->bHasPickableObject = false;
			}
		}
	}
}

