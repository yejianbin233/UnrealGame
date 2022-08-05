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
#include "UnrealGame/PlayerController/BlasterPlayerController.h"

// Sets default values
AWeapon::AWeapon()
{

	// Actor 启用复制
	bReplicates = true;
	
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	WeaponMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	
	SetRootComponent(WeaponMeshComponent);
}

void AWeapon::Init(ABlasterCharacter* InPlayerCharacter, ABlasterPlayerController* InPlayerController)
{
	PlayerCharacter = InPlayerCharacter;
	PlayerController = InPlayerController;
	
	UUnrealGameAssetManager* AssetManager = UUnrealGameAssetManager::Get();

	UWeaponAsset* WeaponAsset = AssetManager->GetPrimaryAssetObject<UWeaponAsset>(AssetId);

	if (!WeaponAsset)
	{
		AssetManager->ForceLoadItem(AssetId);
		WeaponAsset = AssetManager->GetPrimaryAssetObject<UWeaponAsset>(AssetId);
		if (!WeaponAsset)
		{
			UE_LOG(LogTemp, Warning, TEXT("Not Found Weapon Asset Data!"));
			return;
		}
	}

	WeaponMeshComponent->SetSkeletalMesh(WeaponAsset->WeaponMesh);
	WeaponMeshComponent->SetAnimInstanceClass(WeaponAsset->AnimInstanceClass);
	EquippedWeaponSocketName = WeaponAsset->EquippedWeaponSocketName;
	EquippedMontage = WeaponAsset->EquippedMontage;
	PlayerEquipState = WeaponAsset->PlayerEquipState;
}

void AWeapon::Equipment(bool Equipped)
{
	// do somethinig
}

void AWeapon::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, PlayerEquipState);
}
