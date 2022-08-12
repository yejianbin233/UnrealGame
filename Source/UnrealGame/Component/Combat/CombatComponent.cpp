// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"

#include "UnrealGame/Character/BlasterCharacter.h"
#include "UnrealGame/Weapon/Weapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "UnrealGame/Backpack/BackpackComponent.h"
#include "UnrealGame/Component/Aim/AimComponent.h"
#include "UnrealGame/DataAsset/UnrealGameAssetManager.h"
#include "UnrealGame/Backpack/ItemBase.h"
#include "UnrealGame/Weapon/LongRangeWeapon/LongRangeWeapon.h"
#include "UnrealGame/Weapon/ThrowableWeapon/ThrowableWeapon.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(true);
	// ...
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, PlayerCharacter);
	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, LongRangeWeapon);
	DOREPLIFETIME(UCombatComponent, MeleeWeapon);
	DOREPLIFETIME(UCombatComponent, ThrewableWeapon);
	DOREPLIFETIME(UCombatComponent, bIsAiming);
}



// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	// 加载相关资产
	LoadAsset();

}


// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCombatComponent::SC_Equipment_Implementation(AItemBase* EquipItem, float EquipTime)
{
	for (auto PickableObject : PlayerCharacter->PickableObjects)
	{
		if (PickableObject == EquipItem)
		{
			EPickupResult PickupResult = PlayerCharacter->GetBackpackComponent()->Pickup(PickableObject);
			
			if (PickupResult == EPickupResult::All)
			{
				PickableObject->Destroy();
				SNC_Equipment(EquipItem);
				PlayerCharacter->GetBackpackComponent()->OnServerReportBackpackItemChanged.Broadcast(EquipTime);
			}
		}
	}
}

void UCombatComponent::SNC_Equipment(AItemBase* Item)
{
	// note：在 Pickup() 时会自动重置数据，因此在该处先获取数据
	if (!Item)
	{
		return;
	}
	
	FString Id = Item->SceneItemInfo.Id;
	// 装备
	FEquipmentInfo EquipmentInfo = PlayerCharacter->GetEquipmentActorClass(FName(Id));

	FVector Location = PlayerCharacter->GetActorLocation();
	FActorSpawnParameters EquipmentWeaponSpawnParameters;
	EquipmentWeaponSpawnParameters.Owner = GetOwner();
	EquipmentWeaponSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	AWeapon* EquipmentWeapon = GetWorld()->SpawnActor<AWeapon>(EquipmentInfo.EquipmentBase, Location, FRotator(0), EquipmentWeaponSpawnParameters);

	UE_LOG(LogTemp, Warning, TEXT("123"));
	ABlasterCharacter::DisplayRole(EquipmentWeapon->GetLocalRole());
	if (EquipmentWeapon)
	{
		EquipmentWeapon->Init(PlayerCharacter);

		if (PlayerCharacter)
		{
			const USkeletalMeshSocket* EquippedSocket = PlayerCharacter->GetMesh()->GetSocketByName(EquipmentWeapon->GetEquippedWeaponSocket());

			EquippedSocket->AttachActor(EquipmentWeapon, PlayerCharacter->GetMesh());

			EquippedWeapon = EquipmentWeapon;

			// 服务器播放一次，其他客户端的播放由 OnRep 事件处理
			PlayerCharacter->GetMesh()->GetAnimInstance()->Montage_Play(EquippedWeapon->EquippedMontage);
			
			EquippedWeapon->Equipment(true);
		}
	}
}

void UCombatComponent::SNC_UnEquipment()
{
	// 不装备 / 切换武器，将武器自身数据保存到背包武器对应的属性上，如枪填装了子弹，那么将数量更新到背包的枪物品的数量上。
	if (EquippedWeapon)
	{
		EquippedWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}
}

void UCombatComponent::C_Aim_Implementation(bool bToAim)
{
	Aim(bToAim);
}

void UCombatComponent::SC_Aim_Implementation(bool bToAim)
{
	bIsAiming = bToAim;

	C_Aim(bToAim);
}

void UCombatComponent::Aim(bool bToAim)
{
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
	}
}

void UCombatComponent::SNC_Fire()
{
	if (EquippedWeapon == nullptr)
	{
		return;
	}
	
	if (EquippedWeapon->GetWeaponType() == EWeaponType::LongRangeWeapon)
	{
		ALongRangeWeapon* TempLongRangeWeapon = Cast<ALongRangeWeapon>(EquippedWeapon);
		TempLongRangeWeapon->SNC_Fire();
	}
	else if (EquippedWeapon->GetWeaponType() == EWeaponType::ThrowableWeapon)
	{
		AThrowableWeapon* TempThrowableWeapon = Cast<AThrowableWeapon>(EquippedWeapon);
	}
}

void UCombatComponent::CC_Fire_Implementation()
{
	if (EquippedWeapon == nullptr)
	{
		return;
	}
	
	if (EquippedWeapon->GetWeaponType() == EWeaponType::LongRangeWeapon)
	{
		ALongRangeWeapon* TempLongRangeWeapon = Cast<ALongRangeWeapon>(EquippedWeapon);
		TempLongRangeWeapon->CC_Fire();
	}
	else if (EquippedWeapon->GetWeaponType() == EWeaponType::ThrowableWeapon)
	{
		AThrowableWeapon* TempThrowableWeapon = Cast<AThrowableWeapon>(EquippedWeapon);
	}
}

void UCombatComponent::CC_FireHold_Implementation()
{
	if (EquippedWeapon == nullptr)
	{
		return;
	}
	
	if (EquippedWeapon->GetWeaponType() == EWeaponType::LongRangeWeapon)
	{
		ALongRangeWeapon* TempLongRangeWeapon = Cast<ALongRangeWeapon>(EquippedWeapon);
		TempLongRangeWeapon->CC_FireHoldHandle();
	}
	else if (EquippedWeapon->GetWeaponType() == EWeaponType::ThrowableWeapon)
	{
		AThrowableWeapon* TempThrowableWeapon = Cast<AThrowableWeapon>(EquippedWeapon);
	}
}

void UCombatComponent::SNC_FireHold()
{
	if (EquippedWeapon == nullptr)
	{
		return;
	}
	
	if (EquippedWeapon->GetWeaponType() == EWeaponType::LongRangeWeapon)
	{
		ALongRangeWeapon* TempLongRangeWeapon = Cast<ALongRangeWeapon>(EquippedWeapon);
		TempLongRangeWeapon->SNC_FireHoldHandle();
	}
	else if (EquippedWeapon->GetWeaponType() == EWeaponType::ThrowableWeapon)
	{
		AThrowableWeapon* TempThrowableWeapon = Cast<AThrowableWeapon>(EquippedWeapon);
	}
}

void UCombatComponent::CC_FireHoldStop_Implementation()
{
	if (EquippedWeapon == nullptr)
	{
		return;
	}
	
	if (EquippedWeapon->GetWeaponType() == EWeaponType::LongRangeWeapon)
	{
		ALongRangeWeapon* TempLongRangeWeapon = Cast<ALongRangeWeapon>(EquippedWeapon);
		TempLongRangeWeapon->CC_FireHoldStopHandle();
	}
	else if (EquippedWeapon->GetWeaponType() == EWeaponType::ThrowableWeapon)
	{
		AThrowableWeapon* TempThrowableWeapon = Cast<AThrowableWeapon>(EquippedWeapon);
	}
}

void UCombatComponent::SNC_FireHoldStop()
{
	if (EquippedWeapon == nullptr)
	{
		return;
	}
	
	if (EquippedWeapon->GetWeaponType() == EWeaponType::LongRangeWeapon)
	{
		ALongRangeWeapon* TempLongRangeWeapon = Cast<ALongRangeWeapon>(EquippedWeapon);
		TempLongRangeWeapon->SNC_FireHoldStopHandle();
	}
	else if (EquippedWeapon->GetWeaponType() == EWeaponType::ThrowableWeapon)
	{
		AThrowableWeapon* TempThrowableWeapon = Cast<AThrowableWeapon>(EquippedWeapon);
	}
}

void UCombatComponent::CC_Reload_Implementation()
{
	if (EquippedWeapon == nullptr)
	{
		return;
	}
	
	if (EquippedWeapon->GetWeaponType() == EWeaponType::LongRangeWeapon)
	{
		ALongRangeWeapon* TempLongRangeWeapon = Cast<ALongRangeWeapon>(EquippedWeapon);
		TempLongRangeWeapon->CC_Reload();
	}
	else if (EquippedWeapon->GetWeaponType() == EWeaponType::ThrowableWeapon)
	{
		AThrowableWeapon* TempThrowableWeapon = Cast<AThrowableWeapon>(EquippedWeapon);
	}
}

void UCombatComponent::SNC_Reload()
{
	if (EquippedWeapon == nullptr)
	{
		return;
	}
	
	if (EquippedWeapon->GetWeaponType() == EWeaponType::LongRangeWeapon)
	{
		ALongRangeWeapon* TempLongRangeWeapon = Cast<ALongRangeWeapon>(EquippedWeapon);
		TempLongRangeWeapon->SNC_Reload();
	}
	else if (EquippedWeapon->GetWeaponType() == EWeaponType::ThrowableWeapon)
	{
		AThrowableWeapon* TempThrowableWeapon = Cast<AThrowableWeapon>(EquippedWeapon);
	}
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

void UCombatComponent::OnRep_EquippedWeapon()
{
	if (EquippedWeapon == nullptr)
	{
		return;
	}

	if (EquippedWeapon->GetWeaponType() == EWeaponType::LongRangeWeapon)
	{
		PlayerCharacter->GetMesh()->GetAnimInstance()->Montage_Play(EquippedWeapon->EquippedMontage);
	}
	else if (EquippedWeapon->GetWeaponType() == EWeaponType::ThrowableWeapon)
	{
		AThrowableWeapon* TempThrowableWeapon = Cast<AThrowableWeapon>(EquippedWeapon);
	}
	UE_LOG(LogTemp, Warning, TEXT("OnRep Equip Weapon"));
}

void UCombatComponent::LoadAsset()
{
	UUnrealGameAssetManager* AssetManager = UUnrealGameAssetManager::Get();

	for (auto AssetType : LoadAssetType)
	{
		AssetManager->LoadPrimaryAssetsWithType(AssetType);
	}
}
