// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemBase.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "UnrealGame/Character/BlasterCharacter.h"
#include "UnrealGame/ConsoleVariable/ConsoleVariableActor.h"

// Sets default values
AItemBase::AItemBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// 网络复制 Actor
	bReplicates = true;

	// 在构造函数中构造物品的基础框架
	PhysicMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PhysicMeshComponent"));
	// TODO 测试如果组件不复制，而Actor 复制，则会产生的情况
	// PhysicMeshComponent->SetIsReplicated(true);
	
	SetRootComponent(PhysicMeshComponent);

	PickableAreaComponent = CreateDefaultSubobject<USphereComponent>(TEXT("PickableAreaComponent"));
	PickableAreaComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	// PickableAreaComponent->SetIsReplicated(true);

	PickableAreaComponent->OnComponentBeginOverlap.AddDynamic(this, &AItemBase::OnComponentBeginOverlap);
	PickableAreaComponent->OnComponentEndOverlap.AddDynamic(this, &AItemBase::OnComponentEndOverlap);
}

void AItemBase::Init(FItemInfo ItemInfo)
{
	if (bIsInitialized == true)
	{
		return;
	}
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
				SkeletalMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);
				SkeletalMeshComponent->AddWorldTransform(DeltaTransform);
				
				SkeletalMeshComponent->SetSkeletalMesh(ItemInfo.SkeletalMesh);

				ABlasterCharacter::DisplayRole(GetLocalRole());
				FBoxSphereBounds bounds = SkeletalMeshComponent->SkeletalMesh->GetBounds();

				// TODO 为什么在服务器创建的物品，在客户端就缩小了?
				UE_LOG(LogTemp, Warning, TEXT("Size: X%f Y%f Z%f"), bounds.BoxExtent.X, bounds.BoxExtent.Y, bounds.BoxExtent.Z);
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
				StaticMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);
				
				StaticMeshComponent->AddWorldTransform(DeltaTransform);
				
				StaticMeshComponent->SetStaticMesh(ItemInfo.StaticMesh);
				ABlasterCharacter::DisplayRole(GetLocalRole());
				FBoxSphereBounds bounds = StaticMeshComponent->GetStaticMesh()->GetExtendedBounds();
				UE_LOG(LogTemp, Warning, TEXT("Size: X%f Y%f Z%f"), bounds.BoxExtent.X, bounds.BoxExtent.Y, bounds.BoxExtent.Z);
				break;
			}
	}

	ActualItemInfo = GetBackpackItemInfo();
	
	bIsInitialized = true;
}

void AItemBase::CC_PickedUpHandle_Implementation()
{
	// 游戏中隐藏
	this->SetActorHiddenInGame(true);
	// Actor 无碰撞
	this->SetActorEnableCollision(false);
	// 取消模拟物理
	this->PhysicMeshComponent->SetSimulatePhysics(false);
	// 可拾取区域检测组件无碰撞
	this->PickableAreaComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UE_LOG(LogTemp, Warning, TEXT("Client Picked Up Handle Success!"));
}

void AItemBase::CC_CanclePickedUpHandle_Implementation()
{
	ABlasterCharacter::DisplayRole(GetLocalRole());
	// 游戏中显示
	this->SetActorHiddenInGame(false);
	// Actor 碰撞
	this->SetActorEnableCollision(true);
	// 模拟物理
	this->PhysicMeshComponent->SetSimulatePhysics(true);
	// 可拾取区域检测组件碰撞
	this->PickableAreaComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	UE_LOG(LogTemp, Warning, TEXT("Server Picked Up Fail, Client Cancle Picked Up Handle!"));
}

// void AItemBase::SC_UpdateActualItemInfo_Implementation(FBackpackItemInfo NewItemInfo)
// {
// 	ActualItemInfo = NewItemInfo;
// }

// Called when the game starts or when spawned
void AItemBase::BeginPlay()
{
	Super::BeginPlay();

	// 在场景中的物品不会调用 Init，Init 函数是在蓝图类的构造函数中调用的，因此在场景中的物品的都是已经完成初始化的，默认设置为 true。
	bIsInitialized = true;
}

void AItemBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AItemBase, ActualItemInfo);
	DOREPLIFETIME(AItemBase, DeltaTransform);
}

void AItemBase::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bIsInitialized)
	{
		ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
		if (BlasterCharacter)
		{
			BlasterCharacter->GetPickableObjects().Add(this);

			ABlasterCharacter::DisplayRole(GetLocalRole());
			float bIsShow = AConsoleVariableActor::GetEnterOrLeavePickableItemLog();
			if (bIsShow)
			{
				int32 I = 1;
				for (auto Item : BlasterCharacter->GetPickableObjects())
				{
					UE_LOG(LogTemp, Log, TEXT("Add Item(%d)：%s"), I, *Item->GetName());
					I++;
				}
			}
		}
	}
}

void AItemBase::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (bIsInitialized)
	{
		ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
		if (BlasterCharacter)
		{
			BlasterCharacter->GetPickableObjects().Remove(this);

			ABlasterCharacter::DisplayRole(GetLocalRole());
			float bIsShow = AConsoleVariableActor::GetEnterOrLeavePickableItemLog();
			if (bIsShow)
			{
				UE_LOG(LogTemp, Log, TEXT("Remove Item：%s"), *this->GetName());
			}
		}
	}
}

