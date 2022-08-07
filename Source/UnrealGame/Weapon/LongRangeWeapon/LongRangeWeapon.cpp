// Fill out your copyright notice in the Description page of Project Settings.


#include "LongRangeWeapon.h"

#include "../../Casing/Casing.h"
#include "Camera/CameraComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "UnrealGame/Character/BlasterCharacter.h"
#include "UnrealGame/Component/Aim/AimComponent.h"
#include "UnrealGame/Component/Collimation/CollimationComponent.h"
#include "UnrealGame/Component/Collimation/CrosshairComponent.h"
#include "UnrealGame/DataAsset/LongRangeWeaponAsset.h"
#include "UnrealGame/DataAsset/UnrealGameAssetManager.h"
#include "UnrealGame/HUD/Backpack/BackpackComponent.h"
#include "UnrealGame/Projectile/Projectile.h"

ALongRangeWeapon::ALongRangeWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ALongRangeWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ALongRangeWeapon::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void ALongRangeWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void ALongRangeWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CurrentFireInterval -= DeltaSeconds;
	CurrentFireInterval = FMath::Clamp(CurrentFireInterval, 0, FireInterval);
}

void ALongRangeWeapon::InitHandle(ABlasterCharacter* InPlayerCharacter)
{
	Super::InitHandle(InPlayerCharacter);
	
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
	MaxReloadAmmoAmount = LongRangeWeaponAsset->MaxReloadAmmoAmount;
	ProjectileDataTable = LongRangeWeaponAsset->ProjectDataTable;

	// note: 需要 FireInterval 初始化后才能设置自动连续开火的定时器
	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(FireHoldTimerHandle, this, &ALongRangeWeapon::HoldFire, FireInterval, true);
		GetWorld()->GetTimerManager().PauseTimer(FireHoldTimerHandle);

	}
}

void ALongRangeWeapon::Equipment(bool Equipped)
{
	Super::Equipment(Equipped);

	EquipmentHandle(Equipped);
}

void ALongRangeWeapon::HoldFire()
{
	SC_Fire();
}

void ALongRangeWeapon::EquipmentHandle_Implementation(bool Equipped)
{
	if (Equipped)
	{
		CollimationComponent->ShowCollimation();
	}
	else
	{
		CollimationComponent->HideCollimation();
	}
}

void ALongRangeWeapon::SC_Fire_Implementation()
{
	if (CurrentFireInterval > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Shooting too fast! "));
		return;
	}

	if (IsAmmoEmpty())
	{
		// 当手动开火时，如果没有子弹，将播放动画
		PlayCharacterMontage(WeaponAmmoExhaustAnimMontage);
		return;
	}
	
	if (CharacterFireAnimMontage)
	{
		PlayCharacterMontage(CharacterFireAnimMontage);
	}

	if (WeaponFireAnimMontage)
	{
		PlayWeaponMontage(WeaponFireAnimMontage);
	}

	if (bIsPlayWeaponReloadAnimMontage
			|| bIsPlayWeaponAmmoExhaustAnimMontage)
	{
		// 根据播放的蒙太奇来判断是否继续执行"开火"
		return;
	}
	
	SpawnProjectile();

	SpawnCasing();

	// 准星扩散，根据枚举类型判断将转换到哪个子类准星组件
	if (CollimationComponent->GetCollimationType() == ECollimationType::Crosshair)
	{
		UCrosshairComponent* TempCrosshairComponent = Cast<UCrosshairComponent>(CollimationComponent);
		if (TempCrosshairComponent)
		{
			TempCrosshairComponent->C_AddFireCount();
		}
	}
	

	LoadAmmo--;
	UE_LOG(LogTemp, Warning, TEXT("%d"), LoadAmmo);

	CurrentFireInterval = FireInterval;
}

void ALongRangeWeapon::SC_FireHold_Implementation()
{
	// 使用定时器，保持射击
	if (GetWorld()->GetTimerManager().IsTimerPaused(FireHoldTimerHandle))
	{
		UE_LOG(LogTemp, Warning, TEXT("UnPauseTimer"));
		GetWorld()->GetTimerManager().UnPauseTimer(FireHoldTimerHandle);
	} else
	{
		UE_LOG(LogTemp, Warning, TEXT("Active"));
	}
}

void ALongRangeWeapon::SpawnProjectile()
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
					, PlayerCharacter->GetCameraBoom()->GetTargetRotation()
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

void ALongRangeWeapon::PlayMontageStarted(UAnimMontage* AnimMontage)
{
	// 以玩家角色播放的动画为主
	if (AnimMontage == CharacterFireAnimMontage)
	{
		bIsPlayCharacterFireAnimMontage = true;
	}
	else if (AnimMontage == CharacterReloadAnimMontage)
	{
		bIsPlayWeaponReloadAnimMontage = true;
	}
	else if (AnimMontage == WeaponAmmoExhaustAnimMontage)
	{
		bIsPlayWeaponAmmoExhaustAnimMontage = true;
	}
}

void ALongRangeWeapon::PlayMontageEnded(UAnimMontage* AnimMontage, bool bInterrupted)
{
	if (AnimMontage == CharacterFireAnimMontage)
	{
		bIsPlayCharacterFireAnimMontage = false;
	}
	else if (AnimMontage == CharacterReloadAnimMontage)
	{
		bIsPlayWeaponReloadAnimMontage = false;
	}
	else if (AnimMontage == WeaponAmmoExhaustAnimMontage)
	{
		bIsPlayWeaponAmmoExhaustAnimMontage = false;
	}
}

void ALongRangeWeapon::SC_FireHoldStop_Implementation()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(FireHoldTimerHandle))
	{
		GetWorld()->GetTimerManager().PauseTimer(FireHoldTimerHandle);
	}
}

void ALongRangeWeapon::SC_Reload_Implementation()
{
	UBackpackComponent* BackpackComponent = PlayerCharacter->GetBackpackComponent();

	TArray<FBackpackItemInfo> ProjectileItems;
	BackpackComponent->GetItemsByType(EItemType::Projectile, ProjectileItems);

	if (ProjectileItems.Num() == 0)
	{
		return;
	}

	Multicast_Reload();
	
	int TempMaxReloadAmmoAmount = MaxReloadAmmoAmount;

	// TODO - 尚未创建多种子弹，临时使用第一种子弹作为填充的子弹类型
	FBackpackItemInfo ProjectileInfo = ProjectileItems[0];
	
	for (auto ProjectileItem : ProjectileItems)
	{
		int CurReloadNum = FMath::Min(TempMaxReloadAmmoAmount, ProjectileItem.Num);
		TempMaxReloadAmmoAmount -= CurReloadNum;
		ProjectileItem.Num -= CurReloadNum;

		// 更新背包子弹剩余数量
		PlayerCharacter->GetBackpackComponent()->UpdateItemNum(ProjectileItem.BackpackId, ProjectileItem.Num);
		
		if (TempMaxReloadAmmoAmount)
		{
			break;
		}
	}

	if (ProjectileDataTable)
	{
		FString ContentString;
		FProjectileData* DT_ProjectileData = ProjectileDataTable->FindRow<FProjectileData>(FName(ProjectileInfo.Id), ContentString);
		ProjectileData.ProjectileClass = DT_ProjectileData->ProjectileClass;
		ProjectileData.CasingClass = DT_ProjectileData->CasingClass;
	}
	
	LoadAmmo = MaxReloadAmmoAmount - TempMaxReloadAmmoAmount;
}

void ALongRangeWeapon::Multicast_Reload_Implementation()
{
	PlayCharacterMontage(CharacterReloadAnimMontage);
	PlayWeaponMontage(WeaponReloadAnimMontage);
}

bool ALongRangeWeapon::IsAmmoEmpty()
{
	return LoadAmmo <= 0;
}

void ALongRangeWeapon::PlayCharacterMontage_Implementation(UAnimMontage* AnimMontage)
{
	if (AnimMontage == CharacterFireAnimMontage)
	{
		if (bIsPlayCharacterFireAnimMontage
			|| bIsPlayWeaponReloadAnimMontage
			|| bIsPlayWeaponAmmoExhaustAnimMontage)
		{
			return;
		}
	}
	else if (AnimMontage == CharacterReloadAnimMontage)
	{
		if (bIsPlayWeaponReloadAnimMontage)
		{
			return;
		}
	}
	else if (AnimMontage == WeaponAmmoExhaustAnimMontage)
	{
		if (bIsPlayWeaponAmmoExhaustAnimMontage)
		{
			return;
		}
	}
	
	if (PlayerCharacter)
	{
		USkeletalMeshComponent* SkeletalMeshComponent = PlayerCharacter->GetMesh();
		if (SkeletalMeshComponent)
		{
			UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance();
			if (AnimInstance)
			{
				float PlayResult = AnimInstance->Montage_Play(AnimMontage);
				if (PlayResult != 0)
				{
					PlayMontageStarted(AnimMontage);
					FOnMontageEnded BlendingOutDelegate = FOnMontageEnded::CreateUObject(this, &ThisClass::PlayMontageEnded);
					AnimInstance->Montage_SetEndDelegate(BlendingOutDelegate, AnimMontage);
				}
			}
		}
	}
}

void ALongRangeWeapon::PlayWeaponMontage_Implementation(UAnimMontage* AnimMontage)
{
	if (AnimMontage == CharacterFireAnimMontage)
	{
		if (bIsPlayCharacterFireAnimMontage
			|| bIsPlayWeaponReloadAnimMontage
			|| bIsPlayWeaponAmmoExhaustAnimMontage)
		{
			return;
		}
	}
	else if (AnimMontage == CharacterReloadAnimMontage)
	{
		if (bIsPlayWeaponReloadAnimMontage)
		{
			return;
		}
	}
	else if (AnimMontage == WeaponAmmoExhaustAnimMontage)
	{
		if (bIsPlayWeaponAmmoExhaustAnimMontage)
		{
			return;
		}
	}
	
	USkeletalMeshComponent* SkeletalMeshComponent = GetWeaponMeshComponent();
	if (SkeletalMeshComponent)
	{
		UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance();
		if (AnimInstance)
		{
			float PlayResult = AnimInstance->Montage_Play(AnimMontage);
			if (PlayResult != 0)
			{
				PlayMontageStarted(AnimMontage);
				FOnMontageEnded BlendingOutDelegate = FOnMontageEnded::CreateUObject(this, &ThisClass::PlayMontageEnded);
				AnimInstance->Montage_SetEndDelegate(BlendingOutDelegate, AnimMontage);
			}
		}
	}
}
