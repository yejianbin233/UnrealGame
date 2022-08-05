// Fill out your copyright notice in the Description page of Project Settings.


#include "LongRangeWeapon.h"

#include "../../Casing/Casing.h"
#include "Engine/SkeletalMeshSocket.h"
#include "UnrealGame/Character/BlasterCharacter.h"
#include "UnrealGame/Component/Aim/AimComponent.h"
#include "UnrealGame/Component/Collimation/CollimationComponent.h"
#include "UnrealGame/DataAsset/LongRangeWeaponAsset.h"
#include "UnrealGame/DataAsset/UnrealGameAssetManager.h"
#include "UnrealGame/HUD/Backpack/BackpackComponent.h"
#include "UnrealGame/Projectile/Projectile.h"

ALongRangeWeapon::ALongRangeWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
}

bool ALongRangeWeapon::IsAmmoEmpty()
{
	return LoadAmmo <= 0;
}

void ALongRangeWeapon::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void ALongRangeWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CurrentFireInterval -= DeltaSeconds;
	CurrentFireInterval = FMath::Clamp(CurrentFireInterval, 0, FireInterval);
}

void ALongRangeWeapon::Init(ABlasterCharacter* InPlayerCharacter, ABlasterPlayerController* InPlayerController)
{
	Super::Init(InPlayerCharacter, InPlayerController);

	if (CollimationComponentClass)
	{
		CollimationComponent = NewObject<UCollimationComponent>(this, CollimationComponentClass);
		
		//重要，否则无法在细节面板中看到组件
		this->AddInstanceComponent(CollimationComponent);

		//重要，必须要注册组件
		CollimationComponent->RegisterComponent();

		CollimationComponent->Init(GetPlayerCharacter());
		
		if (!CollimationComponent)
		{
			UE_LOG(LogTemp, Warning, TEXT("CollimationComponent Create Fail!"));
		} else
		{
			UE_LOG(LogTemp, Warning, TEXT("CollimationComponent: %s"), *CollimationComponent->GetName());
		}
	}

	if (AimComponentClass)
	{
		AimComponent = NewObject<UAimComponent>(GetPlayerCharacter(), AimComponentClass);
		
		//重要，否则无法在细节面板中看到组件
		this->AddInstanceComponent(AimComponent);

		//重要，必须要注册组件
		AimComponent->RegisterComponent();

		if (!AimComponent)
		{
			UE_LOG(LogTemp, Warning, TEXT("CollimationComponent Create Fail!"));
		} else
		{
			UE_LOG(LogTemp, Warning, TEXT("CollimationComponent: %s"), *CollimationComponent->GetName());
		}
	}

	UUnrealGameAssetManager* AssetManager = UUnrealGameAssetManager::Get();
	
	ULongRangeWeaponAsset* LongRangeWeaponAsset = AssetManager->GetPrimaryAssetObject<ULongRangeWeaponAsset>(AssetId);

	if (!LongRangeWeaponAsset)
	{
		AssetManager->ForceLoadItem(AssetId);
		LongRangeWeaponAsset = AssetManager->GetPrimaryAssetObject<ULongRangeWeaponAsset>(AssetId);
		if (!LongRangeWeaponAsset)
		{
			UE_LOG(LogTemp, Warning, TEXT("Not Found Weapon Asset Data!"));
			return;
		}
	}

	WeaponFireAnimMontage = LongRangeWeaponAsset->WeaponFireAnimMontage;
	CharacterFireAnimMontage = LongRangeWeaponAsset->CharacterFireAnimMontage;
	WeaponReloadAnimMontage = LongRangeWeaponAsset->WeaponReloadAnimMontage;
	CharacterReloadAnimMontage = LongRangeWeaponAsset->CharacterReloadAnimMontage;
	WeaponAmmoExhaustAnimMontage = LongRangeWeaponAsset->WeaponAmmoExhaustAnimMontage;
	FireProjectileSocketName = LongRangeWeaponAsset->FireProjectileSocketName;
	FireInterval = LongRangeWeaponAsset->FireInterval;
	FireOverHeatCurve = LongRangeWeaponAsset->FireOverHeatCurve;
	MaxReloadAmmoAmount = LongRangeWeaponAsset->MaxReloadAmmoAmount;
}

void ALongRangeWeapon::Equipment(bool Equipped)
{
	Super::Equipment(Equipped);

	if (Equipped)
	{
		CollimationComponent->ShowCollimation();
	}
	else
	{
		CollimationComponent->HideCollimation();
	}
}

void ALongRangeWeapon::Fire()
{
	if (CurrentFireInterval > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Shooting too fast! "));
		return;
	}
	if (IsAmmoEmpty())
	{
		// 当手动开火时，如果没有子弹，将播放动画
		GetPlayerCharacter()->GetMesh()->PlayAnimation(WeaponAmmoExhaustAnimMontage, false);
		
		return;
	}
	
	if (CharacterFireAnimMontage)
	{
		GetPlayerCharacter()->GetMesh()->PlayAnimation(CharacterFireAnimMontage, false);
	}

	if (WeaponFireAnimMontage)
	{
		WeaponMeshComponent->PlayAnimation(WeaponFireAnimMontage, false);
	}

	SpawnAmmoProjectile();

	// TODO 
	SpawnCasing();

	LoadAmmo--;
}

void ALongRangeWeapon::FireHold()
{
	// 使用定时器，保持射击
	GetWorld()->GetTimerManager().UnPauseTimer(FireHoldTimerHandle);
}

void ALongRangeWeapon::FireButtonReleased()
{
	GetWorld()->GetTimerManager().PauseTimer(FireHoldTimerHandle);
}

void ALongRangeWeapon::Reload()
{
	UBackpackComponent* BackpackComponent = PlayerCharacter->GetBackpackComponent();

	TArray<FBackpackItemInfo> ProjectileItems;
	BackpackComponent->GetItemsByType(EItemType::Projectile, ProjectileItems);

	if (ProjectileItems.Num() == 0)
	{
		return;
	}

	
}

void ALongRangeWeapon::SpawnAmmoProjectile()
{
	if (ProjectileData.ProjectileClass)
	{
		const USkeletalMeshSocket* ProjectileSocket = WeaponMeshComponent->GetSocketByName(FireProjectileSocketName);

		if (ProjectileSocket)
		{
			FTransform SocketTransform = ProjectileSocket->GetSocketTransform(WeaponMeshComponent);

			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Owner = PlayerCharacter;
			SpawnParameters.Instigator = PlayerCharacter;
			
			UWorld* World = GetWorld();
			if (World)
			{
				World->SpawnActor<AProjectile>(
					ProjectileData.ProjectileClass
					, SocketTransform.GetLocation()
					, SocketTransform.GetRotation().Rotator()
					, SpawnParameters
				);
			}
		}
	}
}

void ALongRangeWeapon::SpawnCasing()
{
	if (ProjectileData.CasingClass)
	{
		const USkeletalMeshSocket* AmmoEjectSocket = WeaponMeshComponent->GetSocketByName(FireCasingSocketName);

		if (AmmoEjectSocket)
		{
			FTransform SocketTransform = AmmoEjectSocket->GetSocketTransform(WeaponMeshComponent);

			UWorld* World = GetWorld();
			if (World)
			{
				World->SpawnActor<ACasing>(
					ProjectileData.CasingClass
					, SocketTransform.GetLocation()
					, SocketTransform.GetRotation().Rotator()
				);
			}
		}
	}
}

