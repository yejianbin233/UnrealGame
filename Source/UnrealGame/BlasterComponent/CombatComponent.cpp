// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"

#include "Camera/CameraComponent.h"
#include "UnrealGame/Character/BlasterCharacter.h"
#include "UnrealGame/PlayerController/BlasterPlayerController.h"
#include "UnrealGame/HUD/BlasterHUD.h"
#include "UnrealGame/Weapon/Weapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	BaseWalkSpeed = 600.0f;
	AimWalkSpeed = 450.0f;
	// ...
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bIsAiming);
	DOREPLIFETIME_CONDITION(UCombatComponent, CarriedAmmo, COND_OwnerOnly);
	DOREPLIFETIME(UCombatComponent, CombatState);
}

// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (BlasterCharacter)
	{
		BlasterCharacter->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

		if (BlasterCharacter->GetFollowCamera())
		{
			DefaultFOV = BlasterCharacter->GetFollowCamera()->FieldOfView;
			CurrentFOV = DefaultFOV;
			
		}

		if (BlasterCharacter->HasAuthority())
		{
			InitializeCarriedAmmo();
		}
	}

	

}


// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (BlasterCharacter && BlasterCharacter->IsLocallyControlled())
	{
		// 只需要本地玩家进行测试
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		HitTarget = HitResult.ImpactPoint;

		// 显示准心 HUD
		SetHUDCrosshairs(DeltaTime);

		// 插值 FOV - 使用 Character 的 Aim 输入

		
	}
	
}

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (BlasterCharacter == nullptr || WeaponToEquip == nullptr)
	{
		return;
	}

	if (EquippedWeapon)
	{
		// EquippedWeapon->Dropped();
	}

	EquippedWeapon = WeaponToEquip;
	// EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	const USkeletalMeshSocket* RightHandSocket = BlasterCharacter->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if (RightHandSocket)
	{
		RightHandSocket->AttachActor(EquippedWeapon, BlasterCharacter->GetMesh());
	}
	
	EquippedWeapon->SetOwner(BlasterCharacter);
	// EquippedWeapon->SetHUDAmmo();

	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}

	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(BlasterCharacter->GetController()) : BlasterPlayerController;
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDCarriedAmmo(CarriedAmmo);
	}
	
	BlasterCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
	BlasterCharacter->bUseControllerRotationYaw = true;
}

void UCombatComponent::Reload()
{
	if (CarriedAmmo > 0)
	{
		ServerReload();
	}
	// 1. 播放重装弹蒙太奇
	
	// 2. 
	
	// 3. 
}

void UCombatComponent::ServerReload_Implementation()
{
	if (BlasterCharacter == nullptr || EquippedWeapon == nullptr)
	{
		return;
	}

	CombatState = ECombatState::ECS_Reloading;
	
	HandleReload();
}

void UCombatComponent::OnRep_CombatState()
{
	switch (CombatState)
	{
		case (ECombatState::ECS_Reloading):
			HandleReload();
			break;

		default:
			break;
	}
}

void UCombatComponent::UpdateAmmoValues()
{
	int32 ReloadAmount = AmountToReload();
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmoMap[EquippedWeapon->GetWeaponType()] -= ReloadAmount;
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}

	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(BlasterCharacter->GetController()) : BlasterPlayerController;
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDCarriedAmmo(CarriedAmmo);
	}
	
	// EquippedWeapon->AddAmmo(ReloadAmount);
}

void UCombatComponent::HandleReload()
{
	BlasterCharacter->PlayReloadMontage();
}

int32 UCombatComponent::AmountToReload()
{
	if (EquippedWeapon == nullptr)
	{
		return 0;
	}

	// int32 RoomInMag = EquippedWeapon->GetMaxReloadAmmoAmount() - EquippedWeapon->GetAmmo();
	//
	// if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	// {
	// 	int32 AmountCarried = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	//
	// 	int32 Least = FMath::Min(RoomInMag, AmountCarried);
	//
	// 	return FMath::Clamp(RoomInMag, 0 , Least);
	// }
	
	return 0;
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	if (EquippedWeapon && BlasterCharacter)
	{
		// EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
		const USkeletalMeshSocket* RightHandSocket = BlasterCharacter->GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if (RightHandSocket)
		{
			RightHandSocket->AttachActor(EquippedWeapon, BlasterCharacter->GetMesh());
		}
		// 当拾取武器后，不同自动旋转到移动方向。
		BlasterCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
		// 使用控制区旋转来控制 Character 旋转
		BlasterCharacter->bUseControllerRotationYaw = true;
	}
}


void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if (BlasterCharacter == nullptr || BlasterCharacter->GetController() == nullptr)
	{
		return;
	}

	BlasterPlayerController = BlasterPlayerController == nullptr
		? Cast<ABlasterPlayerController>(BlasterCharacter->GetController()) : BlasterPlayerController;

	if (BlasterPlayerController)
	{
		// 在 GameMode 中设置的 HUD 会自动添加到 PlayerController(玩家控制器) 中??? // TODO
		BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(BlasterPlayerController->GetHUD()) : BlasterHUD;

		if (BlasterHUD)
		{
			if (EquippedWeapon)
			{
				// HUDPackage.CrosshairsCenter = EquippedWeapon->CrosshairsCenter;
				// HUDPackage.CrosshairsTop = EquippedWeapon->CrosshairsTop;
				// HUDPackage.CrosshairsLeft = EquippedWeapon->CrosshairsLeft;
				// HUDPackage.CrosshairsBottom = EquippedWeapon->CrosshairsBottom;
				// HUDPackage.CrosshairsRight = EquippedWeapon->CrosshairsRight;
			}
			else
			{
				HUDPackage.CrosshairsCenter = nullptr;
				HUDPackage.CrosshairsTop = nullptr;
				HUDPackage.CrosshairsLeft = nullptr;
				HUDPackage.CrosshairsBottom = nullptr;
				HUDPackage.CrosshairsRight = nullptr;
			}
			// 根据速度来影响准星偏移
			FVector2D WalkSpeedRange(0.0f, BlasterCharacter->GetCharacterMovement()->MaxWalkSpeed);
			FVector2D VelocityMultiplierRange(0.0f, 1.0f);

			FVector Velocity = BlasterCharacter->GetVelocity();
			Velocity.Z = 0.0f;

			CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

			// 是否处于飞行状态影响准心偏移
			// TODO 偏移调整速度应该可以调整，方便设计扩展更多的功能，带来更好的体验
			if (BlasterCharacter->GetCharacterMovement()->IsFalling())
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
			}
			else
			{
				// 如果不处于飞行状态，逐渐减少影响直到无影响
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.0f, DeltaTime, 30.0f);
			}

			// 根据是否处于瞄准状态来影响准星偏移
			if (bIsAiming)
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.58f, DeltaTime, 30.0f);
			}
			else
			{
				// 如果不瞄准，逐渐减少影响直到无影响
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.0f, DeltaTime, 30.0f);
			}

			// 貌似不需要使用定时器，使用该行代码即可实现射击结束后的准星自动恢复
			CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.0f, DeltaTime, 30.0f);
			// 0.5 为默认基础值
			// TODO 在射击时应逐步增加准心偏移度，并且持续某段时间后到达最大值
			HUDPackage.CrosshairSpread = 0.5f + CrosshairVelocityFactor + CrosshairInAirFactor - CrosshairAimFactor + CrosshairShootingFactor;

			BlasterHUD->SetHUDPackage(HUDPackage);
		}
	}
}

void UCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;

	if (GEngine && GEngine->GameViewport)
	{
		// 获取视口大小
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	// 准星
	FVector2D CrosshairLocation(ViewportSize.X / 2.0f, ViewportSize.Y / 2.0f);

	FVector CrosshairWorldLocation;
	FVector CrosshairWorldDirection;

	// 将视口准星的位置投影到世界空间中
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
			UGameplayStatics::GetPlayerController(this, 0)
			, CrosshairLocation
			, CrosshairWorldLocation
			, CrosshairWorldDirection
		);

	if (bScreenToWorld)
	{
		FVector Start = CrosshairWorldLocation;

		if (BlasterCharacter)
		{
			float DistanceToCharacter = (BlasterCharacter->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (DistanceToCharacter + 100.0f);

			// DrawDebugSphere(GetWorld(), Start, 16.0f, 12, FColor::Red, false);
		}

		FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;

		GetWorld()->LineTraceSingleByChannel(
				TraceHitResult
				, Start
				, End
				, ECollisionChannel::ECC_Visibility
			);

		// 判断 AActor 是否实现了某个接口
		if (TraceHitResult.GetActor() && TraceHitResult.GetActor()->Implements<UInteractWithCrosshairsInterface>())
		{
			HUDPackage.CrosshairsColor = FLinearColor::Red;
		}
		else
		{

			// TODO 应该可定义颜色
			HUDPackage.CrosshairsColor = FLinearColor::White;
		}
	}
}


void UCombatComponent::SetAiming(bool InbIsAiming)
{
	bIsAiming = InbIsAiming;

	if (!BlasterCharacter->HasAuthority())
	{
		ServerSetAiming(InbIsAiming);
	}

	if (BlasterCharacter && bIsAiming)
	{
		BlasterCharacter->GetCharacterMovement()->MaxWalkSpeed = AimWalkSpeed;
	}
}

void UCombatComponent::ServerSetAiming_Implementation(bool InbIsAiming)
{
	bIsAiming = InbIsAiming;
	BlasterCharacter->GetCharacterMovement()->MaxWalkSpeed = AimWalkSpeed;
}

void UCombatComponent::InterpFOV(float DeltaTime)
{
	if (EquippedWeapon == nullptr)
	{
		return;
	}

	if (bIsAiming)
	{
		// CurrentFOV = FMath::FInterpTo(CurrentFOV, EquippedWeapon->GetZoomedFOV(), DeltaTime, EquippedWeapon->GetZoomInterpSpeed());
	}
	
	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, ZoomInterpSpeed);
	}

	if (BlasterCharacter && BlasterCharacter->GetFollowCamera())
	{
		BlasterCharacter->GetFollowCamera()->SetFieldOfView(CurrentFOV);
	}
}

void UCombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed;
	bCanFire = true;
	if (bFireButtonPressed)
	{
		Fire();
	}
}

void UCombatComponent::Fire()
{
	if(CanFire())
	{
		bCanFire = false;
		
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		
		Server_Fire(HitResult.ImpactPoint);

		if (EquippedWeapon)
		{
			CrosshairShootingFactor += 0.1f;
			CrosshairShootingFactor = CrosshairShootingFactor > 0.5f ? 0.5 : CrosshairShootingFactor;
			// TODO 限制 CrosshairShootingFactor 的最大值。
			// TODO 随时间缓慢减少 CrosshairShootingFactor 的值，如果空闲则减少到最小值，使用定时器实现是否可行???(不需要实现了，使用 FMath::FInterpTo 即可)
		}
		StartFireTimer();
	}
}

bool UCombatComponent::CanFire()
{
	if (EquippedWeapon == nullptr)
	{
		return false;
	}

	// return !EquippedWeapon->IsEmpty() || !bCanFire;

	return false;
}

void UCombatComponent::OnRep_CarriedAmmo()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(BlasterCharacter->GetController()) : BlasterPlayerController;
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDCarriedAmmo(CarriedAmmo);
	}
}

void UCombatComponent::InitializeCarriedAmmo()
{
	CarriedAmmoMap.Emplace(EWeaponType::EWT_AssaultRifle, 30);
}

void UCombatComponent::StartFireTimer()
{
	if (EquippedWeapon == nullptr || BlasterCharacter == nullptr)
	{
		return;
	}

	// BlasterCharacter->GetWorldTimerManager().SetTimer(FireTimer, this, &UCombatComponent::FireTimerFinished, EquippedWeapon->FireDelay);
}

void UCombatComponent::FireTimerFinished()
{
	if (EquippedWeapon == nullptr)
	{
		return;
	}
	
	bCanFire = true;
	// if (bFireButtonPressed && EquippedWeapon->bAutomatic)
	// {
	// 	Fire();
	// }
}

void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if (EquippedWeapon == nullptr)
	{
		return;
	}
	
	if (BlasterCharacter && bFireButtonPressed)
	{
		BlasterCharacter->PlayFireMontage(bIsAiming);
		// EquippedWeapon->Fire(TraceHitTarget);
	}
}

void UCombatComponent::Server_Fire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	MulticastFire(TraceHitTarget);
}
