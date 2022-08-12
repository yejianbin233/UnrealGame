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
#include "UnrealGame/Backpack/BackpackComponent.h"
#include "UnrealGame/Projectile/Projectile.h"

ALongRangeWeapon::ALongRangeWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	LagCompensationComponent = CreateDefaultSubobject<ULRWLagCompensationComponent>(TEXT("LagCompensationComponent"));
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

	if (PlayerCharacter)
	{
		if (PlayerCharacter->HasAuthority())
		{
			OnServerLoadAmmoChangedFeedback.AddUObject(LagCompensationComponent, &ULRWLagCompensationComponent::ServerReportLoadAmmoChangedResult);
		}
		else
		{
			OnLoadAmmoChanged.AddUObject(LagCompensationComponent, &ULRWLagCompensationComponent::CacheLoadAmmoChangedData);
		}
	}
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
	FireCasingSocketName = LongRangeWeaponAsset->FireCasingSocketName;
	FireInterval = LongRangeWeaponAsset->FireInterval;
	MaxReloadAmmoAmount = LongRangeWeaponAsset->MaxReloadAmmoAmount;
	ProjectileDataTable = LongRangeWeaponAsset->ProjectDataTable;

	// note: 需要 FireInterval 初始化后才能设置自动连续开火的定时器
	if (HasAuthority())
	{
		// 服务器作为客户端调用 SNC_Fire
		GetWorld()->GetTimerManager().SetTimer(FireHoldTimerHandle, this, &ALongRangeWeapon::SNC_Fire, FireInterval, true);
		GetWorld()->GetTimerManager().PauseTimer(FireHoldTimerHandle);
	}
	if (!HasAuthority())
	{
		// 客户端调用 CC_FireHold
		GetWorld()->GetTimerManager().SetTimer(FireHoldTimerHandle, this, &ALongRangeWeapon::CC_FireHold, FireInterval, true);
		GetWorld()->GetTimerManager().PauseTimer(FireHoldTimerHandle);
	}
	
	
}

void ALongRangeWeapon::Equipment(bool Equipped)
{
	Super::Equipment(Equipped);

	EquipmentHandle(Equipped);
}

void ALongRangeWeapon::CC_Fire_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("CC Fire! "));
	// 请求服务器开火
	float ClientFireTime = GetWorld()->GetTimeSeconds();
	// note：只要客户端触发开火，为了让服务器有相同的反应，应该也触发服务器开火，不管是否满足开火条件
	SC_Fire(ClientFireTime);
	
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
	
	if (bIsPlayWeaponReloadAnimMontage
			|| bIsPlayWeaponAmmoExhaustAnimMontage)
	{
		// 根据播放的蒙太奇来判断是否继续执行"开火"
		return;
	}

	
	OnLoadAmmoChanged.Broadcast(ClientFireTime);
	
	// 播放开火相关动画
	PlayCharacterMontage(CharacterFireAnimMontage);
	PlayWeaponMontage(WeaponFireAnimMontage);

	CC_SpawnProjectile();

	// 准星扩散，根据枚举类型判断将转换到哪个子类准星组件
	if (CollimationComponent->GetCollimationType() == ECollimationType::Crosshair)
	{
		UCrosshairComponent* TempCrosshairComponent = Cast<UCrosshairComponent>(CollimationComponent);
		if (TempCrosshairComponent)
		{
			TempCrosshairComponent->AddFireCount();
		}
	}
	
	LoadAmmo--;
	UE_LOG(LogTemp, Warning, TEXT("CC LoadAmmo: %d"), LoadAmmo);

	CurrentFireInterval = FireInterval;
}

void ALongRangeWeapon::SC_Fire_Implementation(float ClientFireTime)
{
	UE_LOG(LogTemp, Warning, TEXT("SC Fire! "));
	if (CurrentFireInterval > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Shooting too fast! "));
		return;
	}

	if (IsAmmoEmpty())
	{
		// 当手动开火时，如果没有子弹，将播放动画
		PlayCharacterMontageExceptClient(WeaponAmmoExhaustAnimMontage);
		return;
	}
	
	if (bIsPlayWeaponReloadAnimMontage
			|| bIsPlayWeaponAmmoExhaustAnimMontage)
	{
		// 根据播放的蒙太奇来判断是否继续执行"开火"
		return;
	}
	
	// 播放开火相关动画
	PlayCharacterMontageExceptClient(CharacterFireAnimMontage);
	PlayWeaponMontageExceptClient(WeaponFireAnimMontage);
	
	SC_SpawnProjectile();
	
	LoadAmmo--;
	UE_LOG(LogTemp, Warning, TEXT("SC Ammo: %d"), LoadAmmo);

	CurrentFireInterval = FireInterval;
	OnServerLoadAmmoChangedFeedback.Broadcast(ClientFireTime, LoadAmmo);
}

void ALongRangeWeapon::SNC_Fire()
{
	UE_LOG(LogTemp, Warning, TEXT("SNC Fire! "));
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
	
	if (bIsPlayWeaponReloadAnimMontage
			|| bIsPlayWeaponAmmoExhaustAnimMontage)
	{
		// 根据播放的蒙太奇来判断是否继续执行"开火"
		return;
	}
	
	// 播放开火相关动画
	PlayCharacterMontageExceptClient(CharacterFireAnimMontage);
	PlayWeaponMontageExceptClient(WeaponFireAnimMontage);
	
	SNC_SpawnProjectile();

	// 准星扩散，根据枚举类型判断将转换到哪个子类准星组件
	if (CollimationComponent->GetCollimationType() == ECollimationType::Crosshair)
	{
		UCrosshairComponent* TempCrosshairComponent = Cast<UCrosshairComponent>(CollimationComponent);
		if (TempCrosshairComponent)
		{
			TempCrosshairComponent->AddFireCount();
		}
	}
	
	LoadAmmo--;
	UE_LOG(LogTemp, Warning, TEXT("SNC Ammo: %d"), LoadAmmo);
	CurrentFireInterval = FireInterval;
}

void ALongRangeWeapon::SNC_FireHold()
{
	SNC_Fire();
}

void ALongRangeWeapon::CC_FireHold_Implementation()
{
	CC_Fire();
}

void ALongRangeWeapon::CC_FireHoldHandle_Implementation()
{
	// 使用定时器，保持射击
	if (GetWorld()->GetTimerManager().IsTimerPaused(FireHoldTimerHandle))
	{
		UE_LOG(LogTemp, Warning, TEXT("CC_UnPauseTimer"));
		GetWorld()->GetTimerManager().UnPauseTimer(FireHoldTimerHandle);
	}
}

void ALongRangeWeapon::SNC_FireHoldHandle()
{
	if (GetWorld()->GetTimerManager().IsTimerPaused(FireHoldTimerHandle))
	{
		UE_LOG(LogTemp, Warning, TEXT("SNC_UnPauseTimer"));
		GetWorld()->GetTimerManager().UnPauseTimer(FireHoldTimerHandle);
	}
}

void ALongRangeWeapon::CC_FireHoldStopHandle_Implementation()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(FireHoldTimerHandle))
	{
		GetWorld()->GetTimerManager().PauseTimer(FireHoldTimerHandle);
	}
}

void ALongRangeWeapon::SNC_FireHoldStopHandle()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(FireHoldTimerHandle))
	{
		GetWorld()->GetTimerManager().PauseTimer(FireHoldTimerHandle);
	}
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

// 根据客户端生成子弹的时间，来处理服务器生成子弹的逻辑
void ALongRangeWeapon::CC_SpawnProjectile_Implementation()
{
	SpawnProjectile();
	SpawnCasing();
}

void ALongRangeWeapon::SC_SpawnProjectile_Implementation()
{
	NM_SpawnProjectileExceptClient();
}

void ALongRangeWeapon::SNC_SpawnProjectile()
{
	NM_SpawnProjectile();
}

void ALongRangeWeapon::NM_SpawnProjectile_Implementation()
{
	SpawnProjectile();
	SpawnCasing();
}

void ALongRangeWeapon::NM_SpawnProjectileExceptClient_Implementation()
{
	ABlasterCharacter::DisplayRole(PlayerCharacter->GetLocalRole());
	if (PlayerCharacter->GetLocalRole() == ROLE_AutonomousProxy)
	{
		return;
	}
	SpawnProjectile();
	SpawnCasing();
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
			SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			
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

			FActorSpawnParameters CasingSpawnParameters;
			CasingSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			ACasing* Casing = GetWorld()->SpawnActor<ACasing>(ProjectileData.CasingClass
				, SocketTransform.GetLocation()
				, SocketTransform.GetRotation().Rotator()
				, CasingSpawnParameters);
			if (Casing)
			{
				UE_LOG(LogTemp, Warning, TEXT("Casing Spawn Success!"));
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

void ALongRangeWeapon::CC_Reload_Implementation()
{
	// 请求服务器填装子弹
	const float ReloadAmmoChangedTime = GetWorld()->GetTimeSeconds();
	SC_Reload(ReloadAmmoChangedTime);
	
	if (LoadAmmo == MaxReloadAmmoAmount)
	{
		UE_LOG(LogTemp, Warning, TEXT("不需要填充子弹!"));
		return;
	}
	
	UBackpackComponent* BackpackComponent = PlayerCharacter->GetBackpackComponent();

	TArray<FBackpackItemInfo> ProjectileItems;
	BackpackComponent->GetItemsByType(EItemType::Projectile, ProjectileItems);

	if (ProjectileItems.Num() == 0)
	{
		return;
	}

	PlayCharacterMontage(CharacterReloadAnimMontage);
	PlayWeaponMontage(WeaponReloadAnimMontage);

	int TempMaxReloadAmmoAmount = MaxReloadAmmoAmount;

	// TODO - 尚未创建多种子弹，临时使用第一种子弹作为填充的子弹类型
	FBackpackItemInfo ProjectileInfo = ProjectileItems[0];
	
	for (auto ProjectileItem : ProjectileItems)
	{
		int CurReloadNum = FMath::Min(TempMaxReloadAmmoAmount, ProjectileItem.Num);
		TempMaxReloadAmmoAmount -= CurReloadNum;
		ProjectileItem.Num -= CurReloadNum;

		// 更新背包子弹剩余数量
		PlayerCharacter->GetBackpackComponent()->CC_UpdateItemNum(ProjectileItem.BackpackId, ProjectileItem.Num);
		
		if (TempMaxReloadAmmoAmount)
		{
			break;
		}
	}

	CC_SetProjectileData(*ProjectileInfo.Id);
	
	LoadAmmo = MaxReloadAmmoAmount - TempMaxReloadAmmoAmount;

	ABlasterCharacter::DisplayRole(GetLocalRole());
	
	OnLoadAmmoChanged.Broadcast(ReloadAmmoChangedTime);
}

void ALongRangeWeapon::SC_Reload_Implementation(float ClientReloadTime)
{
	if (LoadAmmo == MaxReloadAmmoAmount)
	{
		UE_LOG(LogTemp, Warning, TEXT("不需要填充子弹!"));
		return;
	}
	
	UBackpackComponent* BackpackComponent = PlayerCharacter->GetBackpackComponent();

	TArray<FBackpackItemInfo> ProjectileItems;
	BackpackComponent->GetItemsByType(EItemType::Projectile, ProjectileItems);

	if (ProjectileItems.Num() == 0)
	{
		// 服务器反馈填装子弹
    	OnServerLoadAmmoChangedFeedback.Broadcast(ClientReloadTime, LoadAmmo);
		return;
	}
	
	PlayCharacterMontageExceptClient(CharacterReloadAnimMontage);
	PlayWeaponMontageExceptClient(WeaponReloadAnimMontage);

	int TempMaxReloadAmmoAmount = MaxReloadAmmoAmount;

	// TODO - 尚未创建多种子弹，临时使用第一种子弹作为填充的子弹类型
	FBackpackItemInfo ProjectileInfo = ProjectileItems[0];
	
	for (auto ProjectileItem : ProjectileItems)
	{
		int CurReloadNum = FMath::Min(TempMaxReloadAmmoAmount, ProjectileItem.Num);
		TempMaxReloadAmmoAmount -= CurReloadNum;
		ProjectileItem.Num -= CurReloadNum;

		// 更新背包子弹剩余数量
		PlayerCharacter->GetBackpackComponent()->SC_UpdateItemNum(ProjectileItem.BackpackId, ProjectileItem.Num, ClientReloadTime);
		
		if (TempMaxReloadAmmoAmount == 0)
		{
			break;
		}
	}

	SC_SetProjectileData(*ProjectileInfo.Id);
	
	LoadAmmo = MaxReloadAmmoAmount - TempMaxReloadAmmoAmount;

	// 服务器反馈填装子弹
	OnServerLoadAmmoChangedFeedback.Broadcast(ClientReloadTime, LoadAmmo);
}

void ALongRangeWeapon::SNC_Reload()
{
	if (!HasAuthority())
	{
		return;
	}
	if (LoadAmmo == MaxReloadAmmoAmount)
	{
		UE_LOG(LogTemp, Warning, TEXT("不需要填充子弹!"));
		return;
	}

	UBackpackComponent* BackpackComponent = PlayerCharacter->GetBackpackComponent();

	TArray<FBackpackItemInfo> ProjectileItems;
	BackpackComponent->GetItemsByType(EItemType::Projectile, ProjectileItems);

	if (ProjectileItems.Num() == 0)
	{
		return;
	}

	PlayCharacterMontageExceptClient(CharacterReloadAnimMontage);
	PlayWeaponMontageExceptClient(WeaponReloadAnimMontage);

	int TempMaxReloadAmmoAmount = MaxReloadAmmoAmount;

	// TODO - 尚未创建多种子弹，临时使用第一种子弹作为填充的子弹类型
	FBackpackItemInfo ProjectileInfo = ProjectileItems[0];
	
	for (auto ProjectileItem : ProjectileItems)
	{
		int CurReloadNum = FMath::Min(TempMaxReloadAmmoAmount, ProjectileItem.Num);
		TempMaxReloadAmmoAmount -= CurReloadNum;
		ProjectileItem.Num -= CurReloadNum;

		// 更新背包子弹剩余数量
		PlayerCharacter->GetBackpackComponent()->SNC_UpdateItemNum(ProjectileItem.BackpackId, ProjectileItem.Num);
		
		if (TempMaxReloadAmmoAmount)
		{
			break;
		}
	}

	SetProjectileData(*ProjectileInfo.Id);
	
	LoadAmmo = MaxReloadAmmoAmount - TempMaxReloadAmmoAmount;
}

void ALongRangeWeapon::CC_SetProjectileData_Implementation(FName ProjectileId)
{
	SetProjectileData(ProjectileId);
}

void ALongRangeWeapon::SC_SetProjectileData_Implementation(FName ProjectileId)
{
	if (PlayerCharacter->GetLocalRole() == ROLE_AutonomousProxy)
	{
		return;
	}
	SetProjectileData(ProjectileId);
}

void ALongRangeWeapon::SetProjectileData(FName ProjectileId)
{
	if (ProjectileDataTable)
	{
		// 子弹的数据，生成子弹类，生成弹壳类
		FString ContentString;
		FProjectileData* DT_ProjectileData = ProjectileDataTable->FindRow<FProjectileData>(ProjectileId, ContentString);
		ProjectileData.ProjectileClass = DT_ProjectileData->ProjectileClass;
		ProjectileData.CasingClass = DT_ProjectileData->CasingClass;
	}
}

bool ALongRangeWeapon::IsAmmoEmpty()
{
	return LoadAmmo <= 0;
}

void ALongRangeWeapon::PlayCharacterMontage(UAnimMontage* AnimMontage)
{
	if (!AnimMontage)
	{
		return;
	}
	
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

void ALongRangeWeapon::PlayWeaponMontage(UAnimMontage* AnimMontage)
{
	if (!AnimMontage)
	{
		return;
	}
	
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

void ALongRangeWeapon::PlayCharacterMontageExceptClient_Implementation(UAnimMontage* AnimMontage)
{
	if (GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
	{
		// 客户端的动画默认已播放，服务器多播不再处理客户端的蒙太奇播放
		return;
	}

	PlayCharacterMontage(AnimMontage);
}

void ALongRangeWeapon::PlayWeaponMontageExceptClient_Implementation(UAnimMontage* AnimMontage)
{
	if (GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
	{
		// 客户端的动画默认已播放，服务器多播不再处理客户端的蒙太奇播放
		return;
	}
	PlayWeaponMontage(AnimMontage);
}
