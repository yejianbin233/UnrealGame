// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"

#include "UnrealGame/Character/BlasterCharacter.h"
#include "UnrealGame/PlayerController/BlasterPlayerController.h"
#include "UnrealGame/Weapon/Weapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "UnrealGame/Component/Aim/AimComponent.h"
#include "UnrealGame/DataAsset/UnrealGameAssetManager.h"
#include "UnrealGame/HUD/Backpack/BackpackComponent.h"
#include "UnrealGame/HUD/Backpack/ItemBase.h"
#include "UnrealGame/Weapon/LongRangeWeapon/LongRangeWeapon.h"
#include "UnrealGame/Weapon/ThrowableWeapon/ThrowableWeapon.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, PlayerCharacter);
	DOREPLIFETIME(UCombatComponent, PlayerController);
	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, LongRangeWeapon);
	DOREPLIFETIME(UCombatComponent, MeleeWeapon);
	DOREPLIFETIME(UCombatComponent, ThrewableWeapon);
	DOREPLIFETIME(UCombatComponent, bIsAiming);
}

void UCombatComponent::SC_Equipment_Implementation()
{
	if (!PlayerCharacter->HasAuthority())
	{
		return;
	}
	// 检测检测
	PlayerCharacter->TracePickableObject(EPickableObjectState::Equip);

	// note：在 Pickup() 时会自动重置数据，因此在该处先获取数据
	AItemBase* Item = Cast<AItemBase>(PlayerCharacter->PickableObjectData.PickableActor);
	if (!Item)
	{
		return;
	}
	FString Id = Item->SceneItemInfo.Id;

	// 拾取
	PlayerCharacter->GetBackpackComponent()->Pickup();

	
	// 装备
	if (PlayerCharacter->PickableObjectData.HandleState == EPickableObjectState::Pickup
		&& PlayerCharacter->PickableObjectData.TargetState == EPickableObjectState::Equip)
	{
		
		PlayerCharacter->GetCombatComponent()->Multicast_Equipment(FName(Id), PlayerCharacter);

		
		PlayerCharacter->PickableObjectData.HandleState = EPickableObjectState::Default;
		PlayerCharacter->PickableObjectData.TargetState = EPickableObjectState::Default;
	}
}

void UCombatComponent::Multicast_Equipment_Implementation(FName Id, ABlasterCharacter* InPlayerCharacter)
{
	// 初始化
	FEquipmentInfo EquipmentInfo= PlayerCharacter->GetEquipmentActorClass(Id);

	AWeapon* EquipmentWeapon = Cast<AWeapon>(GetWorld()->SpawnActor(EquipmentInfo.EquipmentBase));
			
	if (EquipmentWeapon)
	{
		EquipmentWeapon->Init(InPlayerCharacter, Cast<ABlasterPlayerController>(InPlayerCharacter->GetController()));
	}
	
	if (PlayerCharacter)
	{
		const USkeletalMeshSocket* EquippedSocket = PlayerCharacter->GetMesh()->GetSocketByName(EquipmentWeapon->GetEquippedWeaponSocket());

		EquippedSocket->AttachActor(EquipmentWeapon, PlayerCharacter->GetMesh());

		EquippedWeapon = EquipmentWeapon;

		PlayerCharacter->PlayAnimMontage(EquipmentWeapon->EquippedMontage);
			
		EquippedWeapon->Equipment(true);
	}
}


// void UCombatComponent::Equipement_Implementation(AWeapon* WeaponToEquip)
// {
// 	
// }

void UCombatComponent::Aim_Implementation(bool bToAim)
{
	bIsAiming = bToAim;

	if (EquippedWeapon == nullptr)
	{
		return;
	}
	
	if (EquippedWeapon->GetWeaponType() == EWeaponType::LongRangeWeapon)
	{
		ALongRangeWeapon* TempLongRangeWeapon = Cast<ALongRangeWeapon>(EquippedWeapon);
		TempLongRangeWeapon->AimComponent->Aiming(bToAim);
	}
	else if (EquippedWeapon->GetWeaponType() == EWeaponType::ThrowableWeapon)
	{
		AThrowableWeapon* TempThrowableWeapon = Cast<AThrowableWeapon>(EquippedWeapon);
		// TODO
	}
}

void UCombatComponent::Fire_Implementation()
{
	
}

void UCombatComponent::FireHold_Implementation()
{
	
}

void UCombatComponent::Reload_Implementation()
{
	
}

EPlayerEquipState UCombatComponent::GetPlayerEquipState()
{
	if (!EquippedWeapon)
	{
		return EPlayerEquipState::UnArmed;
	}
	else
	{
		return EquippedWeapon->GetPlayerEquipState();
	}
}

void UCombatComponent::OnRep_EquippedWeapon(AWeapon* InEquippedWeapon)
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep_EquippedWeapon"));
	// Equippement(InEquippedWeapon);
}

void UCombatComponent::LoadAsset()
{
	UUnrealGameAssetManager* AssetManager = UUnrealGameAssetManager::Get();

	for (auto AssetType : LoadAssetType)
	{
		AssetManager->LoadPrimaryAssetsWithType(AssetType);
	}
}

// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<ABlasterCharacter>(GetOwner());
	PlayerController = Cast<ABlasterPlayerController>(PlayerCharacter->GetController());

	// 加载相关资产
	
	LoadAsset();

}


// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
