// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "UnrealGame/Character/BlasterCharacter.h"
#include "UnrealGame/Component/Collimation/CollimationComponent.h"
#include "UnrealGame/DataAsset/UnrealGameAssetManager.h"
#include "UnrealGame/DataAsset/WeaponAsset.h"

// Sets default values
AWeapon::AWeapon()
{

	// Actor 启用复制
	bReplicates = true;
	
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	WeaponMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMeshComponent->SetIsReplicated(true);
	SetRootComponent(WeaponMeshComponent);
}

void AWeapon::Init_Implementation()
{
	bIsInit = true;
	InitHandle();
}

void AWeapon::InitHandle()
{
	UUnrealGameAssetManager* AssetManager = UUnrealGameAssetManager::Get();

	// note：需要预加载资产数据
	UWeaponAsset* WeaponAsset = AssetManager->GetPrimaryAssetObject<UWeaponAsset>(AssetId);
	WeaponMeshComponent->SetSkeletalMesh(WeaponAsset->WeaponMesh);
	WeaponMeshComponent->SetAnimInstanceClass(WeaponAsset->AnimInstanceClass);
	EquippedWeaponSocketName = WeaponAsset->EquippedWeaponSocketName;
	EquippedMontage = WeaponAsset->EquippedMontage;
	PlayerEquipState = WeaponAsset->PlayerEquipState;
}

// void AWeapon::NM_UnEquip_Implementation()
// {
// 	
// }
//
// void AWeapon::NM_UnEquipExceptClient_Implementation()
// {
// 	
// }
//
// void AWeapon::UnEquipHandle()
// {
// 	
// }

void AWeapon::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, PlayerCharacter);
	DOREPLIFETIME(AWeapon, AssetId);
	DOREPLIFETIME(AWeapon, WeaponType);
	DOREPLIFETIME(AWeapon, EquippedWeaponSocketName);
	DOREPLIFETIME(AWeapon, EquippedMontage);
	DOREPLIFETIME(AWeapon, PlayerEquipState);
}

void AWeapon::Use(ABlasterCharacter* NewPlayerCharacter)
{
	Super::Use(NewPlayerCharacter);
	
	SetEquptCharacter(NewPlayerCharacter);
	
	if (!bIsInit)
	{
		Init();
	}
}

void AWeapon::Equip()
{
	
}

void AWeapon::UnEquip()
{
	
}

void AWeapon::SetEquptCharacter_Implementation(ABlasterCharacter* NewPlayerCharacter)
{
	if (NewPlayerCharacter)
	{
		this->PlayerCharacter = NewPlayerCharacter;
	}
}

void AWeapon::CC_Equip_Implementation()
{
	
}
//
void AWeapon::SC_Equip_Implementation()
{
	
}
//
// void AWeapon::SNC_Equip_Implementation()
// {
// 	
// }
//
void AWeapon::NM_Equip_Implementation()
{
	
}
//
void AWeapon::NM_EquipExceptClient_Implementation()
{
	
}
//
// void AWeapon::SNC_UnEquip()
// {
// 	
// }
//
// void AWeapon::SC_UnEquip_Implementation()
// {
// 	
// }
//
// void AWeapon::CC_UnEquip_Implementation()
// {
// 	
// }
