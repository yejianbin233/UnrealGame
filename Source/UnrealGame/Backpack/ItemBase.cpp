// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemBase.h"
#include "Components/SphereComponent.h"
#include "Engine/ActorChannel.h"
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
	// TODO 测试如果组件不复制，而 Actor 复制，则会产生的情况
	// PhysicMeshComponent->SetIsReplicated(true);
	
	SetRootComponent(PhysicMeshComponent);

	PickableAreaComponent = CreateDefaultSubobject<USphereComponent>(TEXT("PickableAreaComponent"));
	PickableAreaComponent->SetupAttachment(RootComponent);
	// PickableAreaComponent->SetIsReplicated(true);

	PickableAreaComponent->OnComponentBeginOverlap.AddDynamic(this, &AItemBase::OnComponentBeginOverlap);
	PickableAreaComponent->OnComponentEndOverlap.AddDynamic(this, &AItemBase::OnComponentEndOverlap);
}

bool AItemBase::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	if (ItemInfoObject) WroteSomething |= Channel->ReplicateSubobject(ItemInfoObject , *Bunch, *RepFlags);

	return WroteSomething;
}

void AItemBase::CC_PutTargetLocation_Implementation(FVector TargetLocation)
{
	// 三种调整位置的方式，只有 TeloportTo 成功，其他两个不成功。
	this->TeleportTo(TargetLocation, this->GetActorRotation());
	// this->GetRootComponent()->SetWorldLocation(TargetLocation);
	// this->GetRootComponent()->SetWorldLocationAndRotation(TargetLocation, this->GetActorRotation());
}

void AItemBase::SC_PutTargetLocation_Implementation(FVector TargetLocation)
{
	this->TeleportTo(TargetLocation, this->GetActorRotation());
	NM_PutTargetLocation(TargetLocation);
}

void AItemBase::NM_PutTargetLocation_Implementation(FVector TargetLocation)
{
	this->TeleportTo(TargetLocation, this->GetActorRotation());
}

void AItemBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	Init();
}

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
}

void AItemBase::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
										UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bIsInitialized)
	{
		ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
		if (BlasterCharacter)
		{
			BlasterCharacter->PickableObjects.AddUnique(this);

			ABlasterCharacter::DisplayRole(GetLocalRole());
			float bIsShow = AConsoleVariableActor::GetEnterOrLeavePickableItemLog();
			if (bIsShow)
			{
				int32 I = 1;
				for (auto Item : BlasterCharacter->PickableObjects)
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
			BlasterCharacter->PickableObjects.Remove(this);

			ABlasterCharacter::DisplayRole(GetLocalRole());
			float bIsShow = AConsoleVariableActor::GetEnterOrLeavePickableItemLog();
			if (bIsShow)
			{
				UE_LOG(LogTemp, Log, TEXT("Remove Item：%s"), *this->GetName());
			}
		}
	}
}

void AItemBase::Init()
{
	if (bIsInitialized == true)
	{
		return;
	}

	if (!DT_Item)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, FString::Printf(TEXT("物品数据表未设置！")));
		}
		return;
	}
	
	const FItemInfo* ItemInfo = DT_Item->FindRow<FItemInfo>(Id, nullptr);

	if (!ItemInfo)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, FString::Printf(TEXT("未在数据库中找到物品数据，物品 ID: %s！"), *Id.ToString()));
		}
		return;
	}
	
	switch (ItemInfo->MeshType)
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
				
				SkeletalMeshComponent->SetSkeletalMesh(ItemInfo->SkeletalMesh);

				ABlasterCharacter::DisplayRole(GetLocalRole());
				const FBoxSphereBounds bounds = SkeletalMeshComponent->SkeletalMesh->GetBounds();

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
				
				StaticMeshComponent->SetStaticMesh(ItemInfo->StaticMesh);
				ABlasterCharacter::DisplayRole(GetLocalRole());
				const FBoxSphereBounds bounds = StaticMeshComponent->GetStaticMesh()->GetExtendedBounds();
				UE_LOG(LogTemp, Warning, TEXT("Size: X%f Y%f Z%f"), bounds.BoxExtent.X, bounds.BoxExtent.Y, bounds.BoxExtent.Z);
				break;
			}
	}

	ActualItemInfo.Id = Id.ToString();
	ActualItemInfo.ItemName = ItemInfo->ItemName;
	ActualItemInfo.ItemType = ItemInfo->ItemType;
	ActualItemInfo.Image = ItemInfo->Image;
	ActualItemInfo.OccupyCellXYLength = ItemInfo->OccupyCellXYLength;
	ActualItemInfo.bIsCanStacking = ItemInfo->bIsCanStacking;
	ActualItemInfo.MaxStackingNum = ItemInfo->MaxStackingNum;

	if (ItemInfo->ItemUseClass)
	{
		ItemUseClass = ItemInfo->ItemUseClass;
	}

	ItemInfoObject = NewObject<UItemInfoObject>(this);
	ItemInfoObject->Init(this);
	// 设置初始化成功标记
	bIsInitialized = true;
}

void AItemBase::PutInSceneHandle(FVector PutLocation)
{
	if (this->HasAuthority())
	{
		SC_PutTargetLocation(PutLocation);
		SC_Show();
	}
	else
	{
		CC_PutTargetLocation(PutLocation);
		CC_Show();
	}
}

void AItemBase::PutInBackpackHandle()
{
	if (this->HasAuthority())
	{
		SC_Hide();
	}
	else
	{
		CC_Hide();
	}
}

void AItemBase::Hide()
{
	// 游戏中隐藏
	this->SetActorHiddenInGame(true);
	// Actor 无碰撞
	this->SetActorEnableCollision(false);
	// 取消模拟物理
	this->PhysicMeshComponent->SetSimulatePhysics(false);
	// 可拾取区域检测组件无碰撞
	this->PickableAreaComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AItemBase::SNC_Hide()
{
	Hide();
}

void AItemBase::SC_Hide_Implementation()
{
	Hide();
}

void AItemBase::CC_Hide_Implementation()
{
	Hide();
	SC_Hide();
}

void AItemBase::Show()
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
}

void AItemBase::SNC_Show()
{
	Show();
}

void AItemBase::SC_Show_Implementation()
{
	Show();
}

void AItemBase::CC_Show_Implementation()
{
	Show();
	SC_Show();
}