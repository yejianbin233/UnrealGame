// Fill out your copyright notice in the Description page of Project Settings.


#include "LongRangeWeapon.h"

#include "../../Casing/Casing.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
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

	DOREPLIFETIME(ALongRangeWeapon, ProjectileData);
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

void ALongRangeWeapon::InitHandle()
{
	Super::InitHandle();


	if (CollimationComponentClass)
	{
		CollimationComponent = NewObject<UCollimationComponent>(this, CollimationComponentClass);
		
		//重要，否则无法在细节面板中看到组件
		this->AddInstanceComponent(CollimationComponent);

		//重要，必须要注册组件
		CollimationComponent->RegisterComponent();
		
		if (!CollimationComponent)
		{
			UE_LOG(LogTemp, Warning, TEXT("CollimationComponent Create Fail!"));
		}
	}
	
	if (AimComponentClass)
	{
		AimComponent = NewObject<UAimComponent>(this, AimComponentClass);
		
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

void ALongRangeWeapon::Use(ABlasterCharacter* InPlayerCharacter)
{
	Super::Use(InPlayerCharacter);

	if (InPlayerCharacter->GetLocalRole() == ROLE_Authority)
	{
		// 服务器调用
		SC_Equip();
	}
	else
	{
		CC_Equip();
	}
}

void ALongRangeWeapon::Equip()
{
	Super::Equip();
	
	if (PlayerCharacter)
	{
		// 装备动画
		this->SetActorHiddenInGame(false);

		const USkeletalMeshSocket* EquippedSocket = PlayerCharacter->GetMesh()->GetSocketByName(this->GetEquippedWeaponSocket());
	
		EquippedSocket->AttachActor(this, PlayerCharacter->GetMesh());
	
		// 服务器播放一次，其他客户端的播放由 OnRep 事件处理
		PlayerCharacter->GetMesh()->GetAnimInstance()->Montage_Play(this->EquippedMontage);
		// 取消装备动画，隐藏 UItemUse Actor
		PlayerCharacter->GetMesh()->GetAnimInstance()->Montage_Play(this->EquippedMontage);

		if (PlayerCharacter->GetLocalRole() == ROLE_Authority
			|| PlayerCharacter->GetLocalRole() == ROLE_AutonomousProxy)
		{
			AimComponent->Aiming(true);
			if (APlayerController* PlayerController = Cast<APlayerController>(PlayerCharacter->GetController()))
			{
				CollimationComponent->ShowCollimation(PlayerController);
			}
		}

		// 装备武器则显示武器
		this->SetActorHiddenInGame(false);
	}
}

void ALongRangeWeapon::SetEquptCharacter(ABlasterCharacter* NewPlayerCharacter)
{
	Super::SetEquptCharacter(NewPlayerCharacter);

	if (this->AimComponent)
	{
		this->AimComponent->PlayerCharacter = NewPlayerCharacter;
	}

	
	if (this->CollimationComponent)
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(PlayerCharacter->GetController()))
		{
			this->CollimationComponent->PlayerController = PlayerController;
		}
	}
}

// void ALongRangeWeapon::SNC_Equip()
// {
// 	Super::SNC_Equip();
// 	NM_Equip();
//
// 	// 服务器作为客户端显示瞄准 UI
// 	AimComponent->Aiming(true);
// }
//
void ALongRangeWeapon::SC_Equip()
{
	Super::SC_Equip();

	NM_Equip();
}
//
void ALongRangeWeapon::CC_Equip()
{
	Super::CC_Equip();

	Equip();
	// 客户端显示瞄准 UI
	AimComponent->Aiming(true);

	if (APlayerController* PlayerController = Cast<APlayerController>(PlayerCharacter->GetController()))
	{
		CollimationComponent->ShowCollimation(PlayerController);
	}
	
	SC_Equip();
}
//
void ALongRangeWeapon::NM_Equip()
{
	Super::NM_Equip();

	Equip();
}

void ALongRangeWeapon::NM_EquipExceptClient()
{
	Super::NM_EquipExceptClient();

	if (PlayerCharacter)
	{
		// 客户端已自调用 "Equip"，在多播将其排除
		if (PlayerCharacter->GetLocalRole() == ROLE_AutonomousProxy)
		{
			return;
		}
		Equip();
	}
}

void ALongRangeWeapon::UnEquip()
{
	Super::UnEquip();

	if (PlayerCharacter)
	{
		// 装备动画
		this->SetActorHiddenInGame(false);

		const USkeletalMeshSocket* EquippedSocket = PlayerCharacter->GetMesh()->GetSocketByName(this->GetEquippedWeaponSocket());

		// 将 武器 Actor 分离
		this->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		
		// 服务器播放一次，其他客户端的播放由 OnRep 事件处理
		PlayerCharacter->GetMesh()->GetAnimInstance()->Montage_Play(this->EquippedMontage);
		// 取消装备动画，隐藏 UItemUse Actor
		PlayerCharacter->GetMesh()->GetAnimInstance()->Montage_Play(this->EquippedMontage);

		// 取消装备则隐藏武器
		this->SetActorHiddenInGame(true);

		if (PlayerCharacter->GetLocalRole() == ROLE_Authority
			|| PlayerCharacter->GetLocalRole() == ROLE_AutonomousProxy)
		{
			AimComponent->Aiming(false);
			CollimationComponent->HideCollimation();
		}
	}
}

// void ALongRangeWeapon::SNC_UnEquip()
// {
// 	Super::SNC_UnEquip();
//
// 	NM_UnEquip();
// }
//
// void ALongRangeWeapon::SC_UnEquip()
// {
// 	Super::SC_UnEquip();
// 	
// 	NM_UnEquipExceptClient();
// }
//
// void ALongRangeWeapon::CC_UnEquip()
// {
// 	Super::CC_UnEquip();
//
// 	UnEquip();
// 	SC_UnEquip();
// }
//
// void ALongRangeWeapon::NM_UnEquip()
// {
// 	Super::NM_UnEquip();
//
// 	UnEquip();
// }
//
// void ALongRangeWeapon::NM_UnEquip_Implementation()
// {
// 	Super::NM_UnEquip_Implementation();
//
// 	if (PlayerCharacter)
// 	{
// 		if (PlayerCharacter->GetLocalRole() == ROLE_AutonomousProxy)
// 		{
// 			return;
// 		}
//
// 		UnEquip();
// 	}
// }
//
// void ALongRangeWeapon::UnEquipHandle()
// {
// 	Super::UnEquipHandle();
//
// 	if (HasAuthority())
// 	{
// 		NM_UnEquip();
// 	}
// 	else
// 	{
// 		CC_UnEquip();
// 		SC_UnEquip();
// 	}
// }

void ALongRangeWeapon::NM_EquipmentHandle_Implementation(bool Equipped)
{
	if (PlayerCharacter)
	{
		if (Equipped)
		{
			// 装备动画
			const USkeletalMeshSocket* EquippedSocket = PlayerCharacter->GetMesh()->GetSocketByName(this->GetEquippedWeaponSocket());
	
			EquippedSocket->AttachActor(this, PlayerCharacter->GetMesh());
	
			// 服务器播放一次，其他客户端的播放由 OnRep 事件处理
			PlayerCharacter->GetMesh()->GetAnimInstance()->Montage_Play(this->EquippedMontage);
		}
		else
		{
			// 取消装备动画，隐藏 UItemUse Actor
			PlayerCharacter->GetMesh()->GetAnimInstance()->Montage_Play(this->EquippedMontage);

			this->SetActorHiddenInGame(true);
		}
	}
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

void ALongRangeWeapon::CC_EquipmentHandle_Implementation(bool Equipped)
{
	if (Equipped)
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(PlayerCharacter->GetController()))
		{
			CollimationComponent->ShowCollimation(PlayerController);
		}
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

	TArray<UItemInfoObject*> ProjectileItems;
	BackpackComponent->GetItemsByType(EItemType::Projectile, ProjectileItems);

	if (ProjectileItems.Num() == 0)
	{
		return;
	}

	PlayCharacterMontage(CharacterReloadAnimMontage);
	PlayWeaponMontage(WeaponReloadAnimMontage);

	int TempMaxReloadAmmoAmount = MaxReloadAmmoAmount;

	// TODO - 尚未创建多种子弹，临时使用第一种子弹作为填充的子弹类型
	FBackpackItemInfo ProjectileInfo = ProjectileItems[0]->BackpackItemInfo;
	
	for (auto ProjectileItem : ProjectileItems)
	{
		int CurReloadNum = FMath::Min(TempMaxReloadAmmoAmount, ProjectileInfo.Num);
		TempMaxReloadAmmoAmount -= CurReloadNum;
		ProjectileInfo.Num -= CurReloadNum;

		// 更新背包子弹剩余数量
		TArray<FBackpackItemInfo> BackpackItemInfos;
		BackpackComponent->GetItemsByBackpackId(BackpackItemInfos, ProjectileItem->BackpackId);
		for (auto BackpackItemInfo : BackpackItemInfos)
		{
			BackpackItemInfo.Num = ProjectileInfo.Num;
			PlayerCharacter->GetBackpackComponent()->CC_UpdateItem(ProjectileItem->BackpackId, BackpackItemInfo);
		}
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

	TArray<UItemInfoObject*> ProjectileItems;
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
	FBackpackItemInfo ProjectileInfo = ProjectileItems[0]->BackpackItemInfo;
	
	for (auto ProjectileItem : ProjectileItems)
	{
		int CurReloadNum = FMath::Min(TempMaxReloadAmmoAmount, ProjectileInfo.Num);
		TempMaxReloadAmmoAmount -= CurReloadNum;
		ProjectileInfo.Num -= CurReloadNum;

		// 更新背包子弹剩余数量
		TArray<FBackpackItemInfo> BackpackItemInfos;
		BackpackComponent->GetItemsByBackpackId(BackpackItemInfos, ProjectileItem->BackpackId);
		for (auto BackpackItemInfo : BackpackItemInfos)
		{
			BackpackItemInfo.Num = ProjectileInfo.Num;
			PlayerCharacter->GetBackpackComponent()->SC_UpdateItem(ProjectileItem->BackpackId, BackpackItemInfo, ClientReloadTime);
		}
		
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

	TArray<UItemInfoObject*> ProjectileItems;
	BackpackComponent->GetItemsByType(EItemType::Projectile, ProjectileItems);

	if (ProjectileItems.Num() == 0)
	{
		return;
	}

	PlayCharacterMontageExceptClient(CharacterReloadAnimMontage);
	PlayWeaponMontageExceptClient(WeaponReloadAnimMontage);

	int TempMaxReloadAmmoAmount = MaxReloadAmmoAmount;

	// TODO - 尚未创建多种子弹，临时使用第一种子弹作为填充的子弹类型
	FBackpackItemInfo ProjectileInfo = ProjectileItems[0]->BackpackItemInfo;
	
	for (auto ProjectileItem : ProjectileItems)
	{
		int CurReloadNum = FMath::Min(TempMaxReloadAmmoAmount, ProjectileInfo.Num);
		TempMaxReloadAmmoAmount -= CurReloadNum;
		ProjectileInfo.Num -= CurReloadNum;

		// 更新背包子弹剩余数量
		TArray<FBackpackItemInfo> BackpackItemInfos;
		BackpackComponent->GetItemsByBackpackId(BackpackItemInfos, ProjectileItem->BackpackId);
		for (auto BackpackItemInfo : BackpackItemInfos)
		{
			BackpackItemInfo.Num = ProjectileInfo.Num;
			PlayerCharacter->GetBackpackComponent()->SNC_UpdateItem(ProjectileItem->BackpackId, BackpackItemInfo);
		}
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
