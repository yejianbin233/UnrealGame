// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "UnrealGame/UnrealGame.h"
#include "UnrealGame/BlasterComponent/CombatComponent.h"
#include "UnrealGame/GameMode/BlasterGameMode.h"
#include "UnrealGame/PlayerController/BlasterPlayerController.h"
#include "UnrealGame/Weapon/Weapon.h"
#include "UnrealGame/PlayerState/BlasterPlayerState.h"
#include "EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "EnhancedInput/Public/EnhancedInputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UnrealGame/HUD/Backpack/BackpackComponent.h"
#include "UnrealGame/HUD/Backpack/BackpackWidget.h"
#include "UnrealGame/HUD/Backpack/ItemBase.h"

ABlasterCharacter::ABlasterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600.0f;
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	// 附加到弹簧臂的末端 Socket
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	// 创建头顶内容控件
	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat"));
	Combat->SetIsReplicated(true);

	BackpackComponent = CreateDefaultSubobject<UBackpackComponent>(TEXT("BackpackComponent"));
	BackpackComponent->SetIsReplicated(true);
	
	// 设置移动组件可蹲伏
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	// 设置忽略摄像机碰撞
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	// 设置自定义碰撞对象响应类型
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	// 网络更新
	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;

	// 溶解
	DissolveTimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimelineComponent"));

}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(ABlasterCharacter, Health);
	DOREPLIFETIME(ABlasterCharacter, bDisableGameplay);
	
}

void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (Combat)
	{
		Combat->BlasterCharacter = this;
	}
}

// Called when the game starts or when spawned
void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();

	UpdateHUDHealth();

	if (HasAuthority())
	{
		// 绑定伤害事件委托
		OnTakeAnyDamage.AddDynamic(this, &ABlasterCharacter::ReceiveDamage);
	}

}

// Called every frame
void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	HideCameraIfCharacterClose();

	PollInit();

	UpdateMovementDirection();

	UpdateJumpToGroundBlend();

	UpdateJumpState();

	UpdateMovementSpeedLevel();

	UpdateMovementRotation();

	UpdateAimOffset(DeltaTime);
	
	TracePickableOjbect();
}

void ABlasterCharacter::PlayFireMontage(bool bIsAiming)
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr)
	{
		return;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName;
		SectionName = bIsAiming ? FName("RifleHip"):FName("RifleAim");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::PlayHitReactMontage()
{

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(HitReactionMontage);
		FName SectionName("FromFront");
		
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::PlayReloadMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr)
	{
		return;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && ReloadMontage)
	{
		AnimInstance->Montage_Play(ReloadMontage);
		FName SectionName;

		switch (Combat->EquippedWeapon->GetWeaponType())
		{
			case EWeaponType::EWT_AssaultRifle:
				SectionName = FName("Rifle");
				break;
			
			default:
				break;
		}
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::MoveForward(const FInputActionValue& ActionValue)
{

	if (bDisableGameplay)
	{
		return;
	}

	float Value = ActionValue.GetMagnitude();
	

	// GEngine->AddOnScreenDebugMessage(
	// 		-1,
	// 		1.0f,
	// 		FColor::Green,
	// 		FString::Printf(TEXT("Move Forward Value: %f"), Value)
	// 	);
	
	if (Controller != nullptr && Value != 0.0f)
	{
		
		const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
		
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));

		// DrawDebugSphere(GetWorld(), GetActorLocation(), 10, 10, FColor::Green, false, 1);
		// DrawDebugSphere(GetWorld(), GetActorLocation()+Direction*100, 10, 10, FColor::Green, false, 1);
		AddMovementInput(Direction, Value);
	}
}

void ABlasterCharacter::MoveRight(const FInputActionValue& ActionValue)
{
	
	if (bDisableGameplay)
	{
		return;
	}

	float Value = ActionValue.GetMagnitude();

	// GEngine->AddOnScreenDebugMessage(
	// 			-1,
	// 			1.0f,
	// 			FColor::Green,
	// 			FString::Printf(TEXT("Move Right Value: %f"), Value)
	// 		);
	
	if (Controller != nullptr && Value != 0.0f)
	{
		const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));

		// DrawDebugSphere(GetWorld(), GetActorLocation(), 10, 10, FColor::Red, false, 1);
		// DrawDebugSphere(GetWorld(), GetActorLocation()+Direction*100, 10, 10, FColor::Red, false, 1);
		AddMovementInput(Direction, Value);
	}
}

void ABlasterCharacter::Turn(const FInputActionValue& ActionValue)
{
	float Value = ActionValue.GetMagnitude() * MouseTurnRate;
	// GEngine->AddOnScreenDebugMessage(
	// 		-1,
	// 		1.0f,
	// 		FColor::Green,
	// 		FString::Printf(TEXT("Turn Value: %f"), Value)
	// 	);
	AddControllerYawInput(Value);
}

void ABlasterCharacter::LookUp(const FInputActionValue& ActionValue)
{
	// 负数向上看
	// 正数向下看
	float Value = ActionValue.GetMagnitude() * MouseLookupRate;

	// TODO - 用户友好优化
	float TempMaxLowPitch = 360 - MaxLowPitch;
	
	float NextPitch = GetControlRotation().Pitch + Value;
	
	// 向上 / 向下 由 NextPitch 和 CurrentPitch 的运算决定，当 NextPitch 大于 CurrentPitch 时，向上；否则向下
	// float Direction = NextPitch - CurrentPitch;
	
	// 上半边 - 当在最上边时，不允许继续向上
	// TODO - Bug - 已存在逻辑，当摄像机被身体遮挡时，隐藏身体，如果将最上边俯视角大于 40 度，且在临界范围突然猛一下向上，就会将身体隐藏。
	if (NextPitch > MaxTopPitch && NextPitch <= 180)
	{
		// 向上
		if (Value < 0)
		{
			Value = 0.0f;
		}
	}
	// 下半边 - 当在最下边时，不允许继续向下
	else if (NextPitch >= 180 && NextPitch <= TempMaxLowPitch)
	{
		// 向下
		if (Value > 0)
		{
			Value = 0.0f;
		}
	}

	AddControllerPitchInput(Value);
	
	// GEngine->AddOnScreenDebugMessage(
	// 		-1,
	// 		1.0f,
	// 		FColor::Green,
	// 		FString::Printf(TEXT("NextPitch Value: %f, Control Pitch: %f"), NextPitch, GetControlRotation().Pitch)
	// 	);
}

void ABlasterCharacter::JumpButtonPressed()
{
	
	if (CanJump())
	{
		Jump();
		bIsJump = true;
	}
}

void ABlasterCharacter::EquipButtonPressed()
{
	if (bDisableGameplay)
	{
		return;
	}
	
	if (Combat)
	{
		if (HasAuthority())
		{
			Combat->EquipWeapon(OverlappingWeapon);
		}
		else
		{
			ServerEquipButtomPressed();
		}
	}
}

void ABlasterCharacter::CrouchButtonPressed()
{
	if (bDisableGameplay)
	{
		return;
	}
	
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void ABlasterCharacter::SprintButtonPressed()
{
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void ABlasterCharacter::SprintButtonReleased()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void ABlasterCharacter::AimButtonPressed()
{
	if (bDisableGameplay)
	{
		return;
	}
	
	if (Combat)
	{
		Combat->SetAiming(true);
	}
}

void ABlasterCharacter::AimButtonReleased()
{
	if (Combat)
	{
		Combat->SetAiming(false);
	}
}

void ABlasterCharacter::FireButtonPressed()
{

	if (bDisableGameplay)
	{
		return;
	}
	
	if (Combat)
	{
		Combat->FireButtonPressed(true);
	}
}

void ABlasterCharacter::FireButtonReleased()
{
	if (bDisableGameplay)
	{
		return;
	}
	
	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}
}

void ABlasterCharacter::ReloadButtonPressed()
{
	if (bDisableGameplay)
	{
		return;
	}
	
	if (Combat)
	{
		Combat->Reload();
	}
}

void ABlasterCharacter::OpenOrCloseBackpack()
{
	if (BackpackComponent)
	{
		BackpackComponent->OpenOrCloseBackpack();
	}
}

void ABlasterCharacter::RotateDragItemWidget()
{
	if (BackpackComponent)
	{
		if (BackpackComponent->BackpackWidget)
		{
			BackpackComponent->BackpackWidget->RotateDragItemWidget();
		}
	}
}

void ABlasterCharacter::Pickup()
{
	bool Successded = false;
	
	if (CurrentPickableActor)
	{
		AItemBase* Item = Cast<AItemBase>(CurrentPickableActor);

		if (ensure(Item))
		{
			// FBackpackItemInfo BackpackItemInfo = NewObject<FBackpackItemInfo>(this, TEXT("BackpackItemInfo"));
			FBackpackItemInfo BackpackItemInfo = Item->GetBackpackItemInfo();
			
			Successded = BackpackComponent->TryAddItem(BackpackItemInfo);

			if (!Successded && BackpackItemInfo.CanRotate())
			{
				// 旋转物品
				BackpackItemInfo.Rotate();
				Successded = BackpackComponent->TryAddItem(BackpackItemInfo);
			}

			if (Successded)
			{
				CurrentPickableActor->Destroy(true);
			}
			else
			{
				if (BackpackItemInfo.bIsRotated)
				{
					// 复原旋转
					BackpackItemInfo.Rotate();
				}
			}
		}
	}
}

void ABlasterCharacter::ServerEquipButtomPressed_Implementation()
{
	if (Combat)
	{
		Combat->EquipWeapon(OverlappingWeapon);
	}
}

void ABlasterCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{

	// 当碰撞重叠时，设置 OverlappingWeapon 并调整可视性
	if (OverlappingWeapon)
	{
		// OverlappingWeapon->ShowPickupWidget(true);
	}
	// 当碰撞重叠结束时，OverlappingWeapon 被设置为 nullptr，那么可以使用 LastWeapon 来设置 OverlappingWeapon 不可视
	else if (LastWeapon)
	{
		// LastWeapon->ShowPickupWidget(false);		
	}
}

void ABlasterCharacter::HideCameraIfCharacterClose()
{
	if (!IsLocallyControlled())
	{
		return;
	}

	if ((FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold)
	{
		GetMesh()->SetVisibility(false);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			// 仅对拥有者不可见
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
	}
	else
	{
		GetMesh()->SetVisibility(true);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			// 仅对拥有者不可见
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
	}
}

void ABlasterCharacter::OnRep_Health()
{

	UpdateHUDHealth();

	// 暂时(如果没有生命恢复，那么 Health 只有减少) - 当生命值改变时调用受击动画蒙太奇，
	PlayHitReactMontage();
}

void ABlasterCharacter::UpdateMovementDirection()
{
	FVector Velocity = GetCharacterMovement()->Velocity;

	if (Velocity.Size() == 0)
	{
		MovementDirection = EMovementDirection::EMD_Idle;

		return;
	}
	

	FRotator ControllerRotate = GetControlRotation();
	FVector UnRotateVelocity = UKismetMathLibrary::LessLess_VectorRotator(Velocity, ControllerRotate);
	FRotator VelocityRotate = UKismetMathLibrary::MakeRotFromX(UnRotateVelocity);

	float Angle = VelocityRotate.Yaw;
	float AbsAngle = FMath::Abs(Angle);

	if (Angle > 0)
	{
		// 右半边
		if (AbsAngle < 15.0f)
		{
			MovementDirection = EMovementDirection::EMD_Forward;
		}
		else if (UKismetMathLibrary::InRange_FloatFloat(AbsAngle, 15.0f, 75.0f, false, false))
		{
			MovementDirection = EMovementDirection::EMD_FR;
		}
		else if(UKismetMathLibrary::InRange_FloatFloat(AbsAngle, 75.0f, 105.0f))
		{
			MovementDirection = EMovementDirection::EMD_Rightward;
		}
		else if (AbsAngle > 165.0f)
		{
			MovementDirection = EMovementDirection::EMD_Backward;
		}
		else
		{
			MovementDirection = EMovementDirection::EMD_BR;
		}
	}
	else
	{
		// 左半边
		if (AbsAngle < 15.0f)
		{
			MovementDirection = EMovementDirection::EMD_Forward;
		}
		else if (UKismetMathLibrary::InRange_FloatFloat(AbsAngle, 15.0f, 75.0f, false, false))
		{
			MovementDirection = EMovementDirection::EMD_FL;
		}
		else if(UKismetMathLibrary::InRange_FloatFloat(AbsAngle, 75.0f, 105.0f))
		{
			MovementDirection = EMovementDirection::EMD_Leftward;
		}
		else if (AbsAngle > 165.0f)
		{
			MovementDirection = EMovementDirection::EMD_Backward;
		} else
		{
			MovementDirection = EMovementDirection::EMD_BL;
		}
	}
}

void ABlasterCharacter::UpdateJumpState()
{
	if (bIsJump)
	{
		bool bIsInAir = IsInAir();
		if (!bIsInAir)
		{
			bIsJump = false;
		}
	}
}

void ABlasterCharacter::UpdateMovementSpeedLevel()
{
	float MaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	if (MaxWalkSpeed == SprintSpeed)
	{
		MovementSpeedLevel = 1;
	} else
	{
		MovementSpeedLevel = 0;
	}
}

void ABlasterCharacter::UpdateMovementRotation()
{

	FollowCamera->bUsePawnControlRotation = false;
	CameraBoom->bUsePawnControlRotation = true;

	if(MovementDirection == EMovementDirection::EMD_Idle)
	{
		// 是否跟随控制器旋转而旋转
		bUseControllerRotationYaw = false;

		// 移动时自动旋转调整至移动方向
		GetCharacterMovement()->bOrientRotationToMovement = false;

	} else
	{
		// 是否跟随控制器旋转而旋转
		bUseControllerRotationYaw = true;

		// 移动时自动旋转调整至移动方向
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}
}

void ABlasterCharacter::UpdateAimOffset(float DeltaTime)
{
	if (MovementDirection == EMovementDirection::EMD_Idle)
	{
		AO_Blend = UKismetMathLibrary::FInterpTo(AO_Blend, 1, DeltaTime, AO_Blend_Speed);
	} else
	{
		AO_Blend = 0.0f;
	}
	
	float ControlYaw = GetControlRotation().Yaw;
	float ControlPitch = GetControlRotation().Pitch;

	if (ControlPitch > 180)
	{
		ControlPitch = ControlPitch - 360.0f;

		float TempMaxLowPitch = MaxLowPitch * -1;
		AO_PitchOffset = UKismetMathLibrary::MapRangeClamped(ControlPitch, TempMaxLowPitch, 0, -90, 0);
	} else
	{
		AO_PitchOffset = UKismetMathLibrary::MapRangeClamped(ControlPitch, 0, MaxTopPitch, 0, 90);
	}

	float ActorYaw = GetActorRotation().Yaw;
	if (ActorYaw < 0)
	{
		ActorYaw += 360.0;
	}

	float RelateControlPitch = ControlYaw - ActorYaw;
	if (RelateControlPitch < 0)
	{
		RelateControlPitch += 360.0f;
	}
	if (RelateControlPitch > 180)
	{
		AO_YawOffset = RelateControlPitch - 360.0f;
	} else
	{
		AO_YawOffset = RelateControlPitch;
	}
	
}

void ABlasterCharacter::TracePickableOjbect()
{
	if (bHasPickableObject)
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetController());

		if (ensure(PlayerController))
		{
			FVector2D ViewportSize;
			GEngine->GameViewport->GetViewportSize(ViewportSize);

			FVector2D CrosshairLocation(ViewportSize.X / 2.0f, ViewportSize.Y / 2.0f);
			
			FVector WorldLocation;
			FVector WorldDirection;
			UGameplayStatics::DeprojectScreenToWorld(PlayerController, CrosshairLocation, WorldLocation, WorldDirection);

			FVector TraceStartLocation = WorldLocation + (WorldDirection * PickupTraceStartOffset);
			FVector TraceEndLocation = WorldLocation + (WorldDirection * PickupTraceDistance);
			TArray<AActor*> IgnoreActors;
			FHitResult HitResult;
			
			UKismetSystemLibrary::SphereTraceSingle(GetWorld(),
				TraceStartLocation,
				TraceEndLocation,
				PickupTraceRadius,
				PickableTraceTypeQuery,
				false,
				IgnoreActors,
				EDrawDebugTrace::ForOneFrame,
				HitResult,
				true);

			if (HitResult.bBlockingHit)
			{
				CurrentPickableActor = HitResult.GetActor();
			} else
			{
				CurrentPickableActor = nullptr;
			}
		}
	}
}

// void ABlasterCharacter::UpdateJumpToGroundBlend()
// {
// 	if (!bWasJumping)
// 	{
// 		return;
// 	}
// 	FVector RootSocketLocation = GetMesh()->GetSocketLocation(FName(TEXT("root_Socket")));
//
// 	FVector TraceDirection = FVector(0, 0, 1);
// 	
// 	FVector TraceEndLocation = RootSocketLocation + TraceDirection * JumpToGroundTraceDistance;
//
// 	FHitResult HitResult;
//
// 	TArray IgnoreActors = TArray<AActor*>();
// 	UKismetSystemLibrary::SphereTraceSingle(GetWorld(),
// 		RootSocketLocation,
// 		TraceEndLocation,
// 		20.0f,
// 		ETraceTypeQuery::TraceTypeQuery1,
// 		false,
// 		IgnoreActors,
// 		EDrawDebugTrace::ForOneFrame, 
// 		HitResult,
// 		true,
// 		FColor::Green,
// 		FColor::Red,
// 		5.0f
// 	);
//
// 	if (!HitResult.bBlockingHit)
// 	{
// 		JumpToGroundBlend = 0.0f;
// 	} else
// 	{
// 		FVector ImpactPointLocation = HitResult.ImpactPoint;
//
// 		float ImpactDistance = UKismetMathLibrary::Vector_Distance(RootSocketLocation, ImpactPointLocation);
//
// 		JumpToGroundBlend = ImpactDistance / JumpToGroundTraceDistance;
// 		JumpToGroundBlend = FMath::Clamp(JumpToGroundBlend, 0, 1);
// 	}
// }

void ABlasterCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (!Weapon && OverlappingWeapon)
	{
		// OverlappingWeapon->ShowPickupWidget(false);
	}
	
	OverlappingWeapon = Weapon;
	// 是否是本地玩家控制
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			// OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

bool ABlasterCharacter::IsWeaponEquipped()
{
	return (Combat && Combat->EquippedWeapon);
}

bool ABlasterCharacter::IsAiming()
{
	return (Combat && Combat->bIsAiming);
}

AWeapon* ABlasterCharacter::GetEquippedWeapon()
{
	if (Combat == nullptr)
	{
		return nullptr;
	}

	return Combat->EquippedWeapon;
}

FVector ABlasterCharacter::GetHitTarget() const
{
	if (Combat == nullptr)
	{
		return FVector();
	}

	return Combat->HitTarget;
}

// Called to bind functionality to input
void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// PlayerInputComponent->BindAxis("MoveForward", this, &ABlasterCharacter::MoveForward);
	// PlayerInputComponent->BindAxis("MoveRight", this, &ABlasterCharacter::MoveRight);
	// PlayerInputComponent->BindAxis("Turn", this, &ABlasterCharacter::Turn);
	// PlayerInputComponent->BindAxis("Lookup", this, &ABlasterCharacter::LookUp);
	//
	//
	// PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ABlasterCharacter::Jump);
	// PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &ABlasterCharacter::EquipButtonPressed);
	// PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ABlasterCharacter::CrouchButtonPressed);
	// PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ABlasterCharacter::AimButtonPressed);
	// PlayerInputComponent->BindAction("Aim", IE_Released, this, &ABlasterCharacter::AimButtonReleased);
	//
	// PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ABlasterCharacter::FireButtonPressed);
	// PlayerInputComponent->BindAction("Fire", IE_Released, this, &ABlasterCharacter::FireButtonReleased);
	// PlayerInputComponent->BindAction("Reload", IE_Released, this, &ABlasterCharacter::ReloadButtonPressed);
	// TODO 使用增强输入 EnhancedInput

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();

			Subsystem->AddMappingContext(ShootGameInputMappingContext, 0);
		}
	}

	if (UEnhancedInputComponent* EInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//... TODO

		if (EIA_Aim)
		{
			// TODO - 不同武器的瞄准由武器决定，这里应该使用委托来处理
			EInputComponent->BindAction(EIA_Aim, ETriggerEvent::Triggered, Combat, &UCombatComponent::Fire);
		}

		if (EIA_Crouch)
		{
			EInputComponent->BindAction(EIA_Crouch, ETriggerEvent::Triggered, this, &ThisClass::CrouchButtonPressed);
		}

		if (EIA_Discard)
		{
			// TODO
		}

		if (EIA_Pickup)
		{
			EInputComponent->BindAction(EIA_Pickup, ETriggerEvent::Triggered, this, &ThisClass::Pickup);
		}

		if (EIA_Equipment)
		{
			// TODO
		}

		if (EIA_Jump)
		{
			EInputComponent->BindAction(EIA_Jump, ETriggerEvent::Triggered, this, &ThisClass::JumpButtonPressed);
		}

		if (EIA_Lookup)
		{
			EInputComponent->BindAction(EIA_Lookup, ETriggerEvent::Triggered, this, &ThisClass::LookUp);
		}

		if (EIA_Shoot)
		{
			// TODO 不需要 FireButtonReleased()
			EInputComponent->BindAction(EIA_Shoot, ETriggerEvent::Triggered, this, &ThisClass::FireButtonPressed);
		}

		if (EIA_Sprint)
		{
			EInputComponent->BindAction(EIA_Sprint, ETriggerEvent::Triggered, this, &ThisClass::SprintButtonPressed);
		}

		if (EIA_UnSprint)
		{
			EInputComponent->BindAction(EIA_UnSprint, ETriggerEvent::Triggered, this, &ThisClass::SprintButtonReleased);
		}

		if (EIA_Turn)
		{
			EInputComponent->BindAction(EIA_Turn, ETriggerEvent::Triggered, this, &ThisClass::Turn);
		}

		if (EIA_MoveForward)
		{
			EInputComponent->BindAction(EIA_MoveForward, ETriggerEvent::Triggered, this, &ThisClass::MoveForward);
		}

		if (EIA_MoveRight)
		{
			EInputComponent->BindAction(EIA_MoveRight, ETriggerEvent::Triggered, this, &ThisClass::MoveRight);
		}

		if (EIA_ReloadAmmo)
		{
			// TODO 
		}

		if (EIA_UseSwitch)
		{
			// TODO
		}

		if (EIA_OpenOrCloseBackpack)
		{
			EInputComponent->BindAction(EIA_OpenOrCloseBackpack, ETriggerEvent::Triggered, this, &ThisClass::OpenOrCloseBackpack);
		}

		if (EIA_RotateDragItemWidget)
		{
			EInputComponent->BindAction(EIA_RotateDragItemWidget, ETriggerEvent::Triggered, this, &ThisClass::RotateDragItemWidget);
		}
	}
	
}

void ABlasterCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatorController, AActor* DamagerCauser)
{
	Health = FMath::Clamp(Health - Damage, 0.0f, MaxHealth);
	
	UpdateHUDHealth();
	PlayHitReactMontage();

	if (Health == 0.0f)
	{
		ABlasterGameMode* BlasterGameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>();
		if (BlasterGameMode)
		{
			BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;

			ABlasterPlayerController* AttackerController = Cast<ABlasterPlayerController>(InstigatorController);
			BlasterGameMode->PlayerEliminated(this, BlasterPlayerController, AttackerController);
		}
	}
	
}

void ABlasterCharacter::UpdateHUDHealth()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;

	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDHealth(Health, MaxHealth);
	}
}

void ABlasterCharacter::PollInit()
{
	if (BlasterPlayerState == nullptr)
	{
		BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();

		if (BlasterPlayerState)
		{
			BlasterPlayerState->AddToScore(0.0f);

			BlasterPlayerState->AddToDefeats(0);
		}
	}
}

void ABlasterCharacter::Elim()
{
	if (Combat && Combat->EquippedWeapon)
	{
		// 如果死亡时装备武器，将丢弃武器
		Combat->EquippedWeapon->Dropped();
	}
	
	bElimmed = true;

	// 各个客户端(如果有监听服务器)都应该播放"被淘汰"的动画
	PlayElimMontage();
	
	// TODO 可以根据模式(游戏模式-GameMode)的不同来触发"被淘汰"后的重生规则
	// 暂时设计被淘汰后一定延迟固定生成
	GetWorldTimerManager().SetTimer(ElimTimer, this, &ABlasterCharacter::ElimTimerFinished, ElimDelay);
}

void ABlasterCharacter::PlayElimMontage_Implementation()
{
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDWeaponAmmo(0);
	}
	
	// 停止移动
	GetCharacterMovement()->DisableMovement();

	// 停止移动及旋转
	GetCharacterMovement()->StopMovementImmediately();

	bDisableGameplay = true;

	// 禁止碰撞
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	// 设置材质实例
	if (DissolverMaterialInstance)
	{
		DynamicDissolveMaterialInstance = UMaterialInstanceDynamic::Create(DissolverMaterialInstance, this);

		GetMesh()->SetMaterial(0, DynamicDissolveMaterialInstance);

		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), 0.55f);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Glow"), 200.0f);
	}

	// 开始溶解
	StartDissolve();
	
	if (AnimInstance && ElimMontage)
	{
		AnimInstance->Montage_Play(ElimMontage);
	}
}

void ABlasterCharacter::ElimTimerFinished()
{
	ABlasterGameMode* BlasterGameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>();

	if (BlasterGameMode)
	{
		// 请求 GameMode 重新生成角色
		BlasterGameMode->RequestRespawn(this, Controller);
	}
}

void ABlasterCharacter::UpdateDissolveMeterial(float DissolveValue)
{
	if (DynamicDissolveMaterialInstance)
	{
		// 设置溶解量
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue);
	}
}

void ABlasterCharacter::StartDissolve()
{
	// 绑定函数，当 Timeline 激活时调用
	DissolveTrack.BindDynamic(this, &ABlasterCharacter::UpdateDissolveMeterial);

	if (DissolveTimelineComponent && DissolveCurve)
	{
		// 溶解 TimeLine 组件设置时间线，使用溶解曲线
		DissolveTimelineComponent->AddInterpFloat(DissolveCurve, DissolveTrack);

		// 播放(激活)溶解 TimeLine 组件
		DissolveTimelineComponent->Play();
	}
}


ECombatState ABlasterCharacter::GetCombatState() const
{
	if (Combat == nullptr)
	{
		return ECombatState::ECS_MAX;
	}

	return Combat->CombatState;
}