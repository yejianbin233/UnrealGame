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

	SetIsReplicated(true);

	OnBackpackThrowItem.AddUObject(this, &UCombatComponent::BackpackThrowItemHandle);
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
			FPickupResult PickupResult = PlayerCharacter->GetBackpackComponent()->Pickup(PickableObject);
			
			// 只要物品在背包中存在
			if (PickupResult.Result == EPickupResult::AddNewItem_All
				|| PickupResult.Result == EPickupResult::AddNewItem_Part
				|| PickupResult.Result == EPickupResult::StackAdd_All
				|| PickupResult.Result == EPickupResult::StackAdd_Part)
			{
				SNC_Equipment(PickupResult.ItemInfoObject->UseItem);
				PlayerCharacter->GetBackpackComponent()->OnServerReportBackpackItemChanged.Broadcast(EquipTime);
			}
			if (PickupResult.Result == EPickupResult::AddNewItem_All
				|| PickupResult.Result == EPickupResult::StackAdd_All)
			{
				// 放入背包
				EquipItem->PutInBackpackHandle();
			}

			break;
		}
	}
}

void UCombatComponent::SNC_Equipment(AItemUse* Item)
{
	// note：在 Pickup() 时会自动重置数据，因此在该处先获取数据
	if (!Item)
	{
		return;
	}

	if (AWeapon* EquippedWeaponItem = Cast<AWeapon>(Item))
	{
		// EquippedWeapon = EquippedWeaponItem;
		Item->Use(PlayerCharacter);
	}
}

void UCombatComponent::CC_UnEquipment_Implementation()
{
	// 更新背包远程武器子弹数量
	UnEquipment();
	SC_UnEquipment();
}

void UCombatComponent::SC_UnEquipment_Implementation()
{

	// 无法判断是否是服务器作为客户端，一次多播统一取消装备
	// NM_UnEquipmentExceptClient();
	NM_UnEquipment();
}

void UCombatComponent::NM_UnEquipmentExceptClient_Implementation()
{
	if (PlayerCharacter->GetLocalRole() == ROLE_AutonomousProxy)
	{
		return;
	}
	UnEquipment();
}

void UCombatComponent::NM_UnEquipment_Implementation()
{
	UnEquipment();
}

void UCombatComponent::SNC_UnEquipment()
{
	// 不装备 / 切换武器，将武器自身数据保存到背包武器对应的属性上，如枪填装了子弹，那么将数量更新到背包的枪物品的数量上。
	NM_UnEquipment();
}

void UCombatComponent::UnEquipment()
{
	if (EquippedWeapon)
	{
		// 取消装备
		EquippedWeapon->UnEquip();
		EquippedWeapon = nullptr;
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

void UCombatComponent::BackpackThrowItemHandle(UItemInfoObject* ThrowItemObject)
{
	if (ThrowItemObject)
	{
		if (AWeapon* ThrowWeaponItem = Cast<AWeapon>(ThrowItemObject->UseItem))
		{
			// 如果丢掉了正在装备武器那么将取消装备，并将武器上的数据白柳
			if (EquippedWeapon == ThrowWeaponItem)
			{
				if (PlayerCharacter->HasAuthority())
				{
					// 服务器取消装备
					SC_UnEquipment();
				}
				else
				{
					// 客户端取消装备
					CC_UnEquipment();
				}
			}
		}
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
