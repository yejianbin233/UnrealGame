// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "Casing.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "UnrealGame/Character/BlasterCharacter.h"
#include "UnrealGame/PlayerController/BlasterPlayerController.h"

// Sets default values
AWeapon::AWeapon()
{

	// Actor 启用复制
	bReplicates = true;
	
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	// 响应碰撞
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	// 忽略指定通道的碰撞
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	// 默认无碰撞
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	
	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);

	AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(RootComponent);

	
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, WeaponState);
	DOREPLIFETIME(AWeapon, Ammo);
	
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	// 仅在服务器上激活武器的碰撞相关设置
	// if(HasAuthority())
	// {
	// 	
	// }
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereEndOverlap);
	}

	if (PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::OnSphereOverlap(
	UPrimitiveComponent* OverlappedComponent
	, AActor* OtherActor
	, UPrimitiveComponent* OtherComponent
	, int32 OtherBodyIndex
	, bool bFromSweep
	, const FHitResult& SweepResult)
{
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);

	if(BlasterCharacter)
	{
		BlasterCharacter->SetOverlappingWeapon(this);
		
	}
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);

	if(BlasterCharacter)
	{
		BlasterCharacter->SetOverlappingWeapon(nullptr);
	}
}

void AWeapon::OnRep_WeaponState()
{
	switch (WeaponState)
	{
		case EWeaponState::EWS_Equipped:
			ShowPickupWidget(false);
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			WeaponMesh->SetSimulatePhysics(false);
			WeaponMesh->SetEnableGravity(false);
			WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			break;
		case EWeaponState::EWS_Dropped:
			WeaponMesh->SetSimulatePhysics(true);
			WeaponMesh->SetEnableGravity(true);
			WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			break;
		default: ;
	}
}

void AWeapon::SetWeaponState(EWeaponState InWeaponState)
{
	WeaponState = InWeaponState;
	
	switch (WeaponState)
	{
		case EWeaponState::EWS_Equipped:
			ShowPickupWidget(false);
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			WeaponMesh->SetSimulatePhysics(false);
			WeaponMesh->SetEnableGravity(false);
			WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			break;

		case EWeaponState::EWS_Dropped:
			if (HasAuthority())
			{
				AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			}
			WeaponMesh->SetSimulatePhysics(true);
			WeaponMesh->SetEnableGravity(true);
			WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			break;
		default: ;
	}
}

bool AWeapon::IsEmpty()
{
	return Ammo <= 0;
}


void AWeapon::ShowPickupWidget(bool bIsShowWidget)
{
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(bIsShowWidget);
	}
}

void AWeapon::Fire(const FVector& HitTarget)
{
	if (FireAnimtion)
	{
		WeaponMesh->PlayAnimation(FireAnimtion, false);
	}
	
	if (CasingClass)
	{
		const USkeletalMeshSocket* AmmoEjectSocket = GetWeaponMesh()->GetSocketByName(FName("AmmoEject"));

		if (AmmoEjectSocket)
		{
			FTransform SocketTransform = AmmoEjectSocket->GetSocketTransform(GetWeaponMesh());

			FActorSpawnParameters SpawnParameters;
			UWorld* World = GetWorld();
			if (World)
			{
				World->SpawnActor<ACasing>(
					CasingClass
					, SocketTransform.GetLocation()
					, SocketTransform.GetRotation().Rotator()
				);
			}
		}
	}

	SpendRound();
}

void AWeapon::OnRep_Owner()
{
	Super::OnRep_Owner();

	if (Owner == nullptr)
	{
		BlasterOwnerCharacter = nullptr;
		BlasterPlayerOwnerController = nullptr;
	}
	else
	{
		SetHUDAmmo();
	}
}

void AWeapon::SetHUDAmmo()
{
	BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlasterCharacter>(GetOwner()) : BlasterOwnerCharacter;

	if (BlasterOwnerCharacter)
	{
		BlasterPlayerOwnerController = BlasterPlayerOwnerController == nullptr ? Cast<ABlasterPlayerController>(BlasterOwnerCharacter->GetController()) : BlasterPlayerOwnerController;

		if (BlasterPlayerOwnerController)
		{
			BlasterPlayerOwnerController->SetHUDWeaponAmmo(Ammo);
		}
	}
}

void AWeapon::Dropped()
{
	SetWeaponState(EWeaponState::EWS_Dropped);

	// 分离组件
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	WeaponMesh->DetachFromComponent(DetachRules);

	// 设置拥有者为 nullptr
	SetOwner(nullptr);

	// 清除引用
	BlasterOwnerCharacter = nullptr;
	BlasterPlayerOwnerController = nullptr;
}

void AWeapon::AddAmmo(int32 AmmoToAdd)
{
	Ammo = FMath::Clamp(Ammo-AmmoToAdd, 0, MagCapacity);
}

void AWeapon::OnRep_Ammo()
{
	BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlasterCharacter>(GetOwner()) : BlasterOwnerCharacter;

	SetHUDAmmo();
}

void AWeapon::SpendRound()
{
	Ammo = FMath::Clamp(Ammo - 1, 0, MagCapacity);
	
	SetHUDAmmo();
}
