// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "UnrealGame/UnrealGame.h"
#include "UnrealGame/GameMode/BlasterGameMode.h"
#include "UnrealGame/PlayerController/BlasterPlayerController.h"
#include "UnrealGame/Weapon/Weapon.h"
#include "UnrealGame/PlayerState/BlasterPlayerState.h"
#include "EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "EnhancedInput/Public/EnhancedInputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UnrealGame/Component/Combat/CombatComponent.h"
#include "UnrealGame/Backpack/BackpackComponent.h"
#include "UnrealGame/HUD/Backpack/BackpackWidget.h"
#include "UnrealGame/InteractiveActor/InteractiveDoor.h"
#include "DataRegistrySubsystem.h"
#include "Components/BoxComponent.h"
#include "UnrealGame/Backpack/BackpackLagCompensationComponent.h"
#include "UnrealGame/Backpack/ItemBase.h"
#include "UnrealGame/Component/Camera/UnrealCameraComponent.h"
#include "UnrealGame/Component/LagCompensation/PlayerLagCompensationComponent.h"
#include "UnrealGame/ConsoleVariable/ConsoleVariableActor.h"
#include "UnrealGame/DataAsset/UnrealGameAssetManager.h"

ABlasterCharacter::ABlasterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	PlayerCameraComponent = CreateDefaultSubobject<UUnrealCameraComponent>(TEXT("PlayerCameraComponent"));
	PlayerCameraComponent->SetupAttachment(GetMesh());
	
	// 创建头顶内容控件
	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	// Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat"));
	// Combat->SetIsReplicated(true);

	BackpackComponent = CreateDefaultSubobject<UBackpackComponent>(TEXT("BackpackComponent"));
	BackpackComponent->SetIsReplicated(true);

	BackpackLagCompensationComponent = CreateDefaultSubobject<UBackpackLagCompensationComponent>(TEXT("BackpackLagCompensationComponent"));
	BackpackLagCompensationComponent->SetIsReplicated(true);
	
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

	CombatCopmponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));

	/* 玩家角色滞后补偿相关设置 */
	PlayerLagCompensationComponent = CreateDefaultSubobject<UPlayerLagCompensationComponent>(TEXT("PlayerLagCompensationComponent"));
	HeadBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("HeadBoxComponent"));
	HeadBoxComponent->SetupAttachment(GetMesh(), FName(TEXT("head")));
	HeadBoxComponent->ComponentTags.Add(BoneCollisionBoxTagName);
	
	Spine_05_BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Spine_05_BoxComponent"));
	Spine_05_BoxComponent->SetupAttachment(GetMesh(), FName(TEXT("spine_05")));
	Spine_05_BoxComponent->ComponentTags.Add(BoneCollisionBoxTagName);
	
	Spine_03_BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Spine_03_BoxComponent"));
	Spine_03_BoxComponent->SetupAttachment(GetMesh(), FName(TEXT("spine_03")));
	Spine_03_BoxComponent->ComponentTags.Add(BoneCollisionBoxTagName);
	
	Spine_01_BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Spine_01_BoxComponent"));
	Spine_01_BoxComponent->SetupAttachment(GetMesh(), FName(TEXT("spine_01")));
	Spine_01_BoxComponent->ComponentTags.Add(BoneCollisionBoxTagName);
	
	Left_Clavicle_Out_BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Left_Clavicle_Out_BoxComponent"));
	Left_Clavicle_Out_BoxComponent->SetupAttachment(GetMesh(), FName(TEXT("clavicle_out_l")));
	Left_Clavicle_Out_BoxComponent->ComponentTags.Add(BoneCollisionBoxTagName);
	
	Left_Upperarm_BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Left_Upperarm_Out_BoxComponent"));
	Left_Upperarm_BoxComponent->SetupAttachment(GetMesh(), FName(TEXT("upperarm_l")));
	Left_Upperarm_BoxComponent->ComponentTags.Add(BoneCollisionBoxTagName);
	
	Left_Lowerarm_BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Left_Lowerarm_BoxComponent"));
	Left_Lowerarm_BoxComponent->SetupAttachment(GetMesh(), FName(TEXT("lowerarm_l")));
	Left_Lowerarm_BoxComponent->ComponentTags.Add(BoneCollisionBoxTagName);
	
	Left_Hand_BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Left_Hand_BoxComponent"));
	Left_Hand_BoxComponent->SetupAttachment(GetMesh(), FName(TEXT("hand_l")));
	Left_Hand_BoxComponent->ComponentTags.Add(BoneCollisionBoxTagName);
	
	Left_Thigh_BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Left_Thigh_BoxComponent"));
	Left_Thigh_BoxComponent->SetupAttachment(GetMesh(), FName(TEXT("thigh_l")));
	Left_Thigh_BoxComponent->ComponentTags.Add(BoneCollisionBoxTagName);
	
	Left_Calf_BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Left_Calf_BoxComponent"));
	Left_Calf_BoxComponent->SetupAttachment(GetMesh(), FName(TEXT("calf_l")));
	Left_Calf_BoxComponent->ComponentTags.Add(BoneCollisionBoxTagName);
	
	Left_Foot_BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Left_Foot_BoxComponent"));
	Left_Foot_BoxComponent->SetupAttachment(GetMesh(), FName(TEXT("foot_l")));
	Left_Foot_BoxComponent->ComponentTags.Add(BoneCollisionBoxTagName);

	Right_Clavicle_Out_BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Right_Clavicle_Out_BoxComponent"));
	Right_Clavicle_Out_BoxComponent->SetupAttachment(GetMesh(), FName(TEXT("clavicle_out_r")));
	Right_Clavicle_Out_BoxComponent->ComponentTags.Add(BoneCollisionBoxTagName);
	
	Right_Upperarm_BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Right_Upperarm_Out_BoxComponent"));
	Right_Upperarm_BoxComponent->SetupAttachment(GetMesh(), FName(TEXT("upperarm_r")));
	Right_Upperarm_BoxComponent->ComponentTags.Add(BoneCollisionBoxTagName);
	
	Right_Lowerarm_BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Right_Lowerarm_BoxComponent"));
	Right_Lowerarm_BoxComponent->SetupAttachment(GetMesh(), FName(TEXT("lowerarm_r")));
	Right_Lowerarm_BoxComponent->ComponentTags.Add(BoneCollisionBoxTagName);
	
	Right_Hand_BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Right_Hand_BoxComponent"));
	Right_Hand_BoxComponent->SetupAttachment(GetMesh(), FName(TEXT("hand_r")));
	Right_Hand_BoxComponent->ComponentTags.Add(BoneCollisionBoxTagName);
	
	Right_Thigh_BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Right_Thigh_BoxComponent"));
	Right_Thigh_BoxComponent->SetupAttachment(GetMesh(), FName(TEXT("thigh_r")));
	Right_Thigh_BoxComponent->ComponentTags.Add(BoneCollisionBoxTagName);
	
	Right_Calf_BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Right_Calf_BoxComponent"));
	Right_Calf_BoxComponent->SetupAttachment(GetMesh(), FName(TEXT("calf_r")));
	Right_Calf_BoxComponent->ComponentTags.Add(BoneCollisionBoxTagName);
	
	Right_Foot_BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Right_Foot_BoxComponent"));
	Right_Foot_BoxComponent->SetupAttachment(GetMesh(), FName(TEXT("foot_r")));
	Right_Foot_BoxComponent->ComponentTags.Add(BoneCollisionBoxTagName);
	/* 玩家角色滞后补偿相关设置 */
}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterCharacter, Health);
	DOREPLIFETIME(ABlasterCharacter, bDisableGameplay);
	
	DOREPLIFETIME(ABlasterCharacter, bIsJump);
	DOREPLIFETIME(ABlasterCharacter, bElimmed);
	DOREPLIFETIME(ABlasterCharacter, MovementDirection);
	DOREPLIFETIME(ABlasterCharacter, MovementSpeedLevel);
	DOREPLIFETIME(ABlasterCharacter, AO_YawOffset);
	DOREPLIFETIME(ABlasterCharacter, AO_PitchOffset);
	DOREPLIFETIME(ABlasterCharacter, AO_Blend);
	
}

void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ABlasterCharacter::Interactive_Implementation()
{
	if (!HasAuthority())
	{
		return;
	}
	// 1. 先进行射线检测
	FVector CameraLocation = PlayerCameraComponent->GetCameraComponent()->GetComponentLocation();
	FVector CameraForwardDirectional = PlayerCameraComponent->GetCameraComponent()->GetForwardVector();
	FVector CameraTraceEndLocation = CameraLocation + (CameraForwardDirectional * InteractiveTraceDistance);
	FHitResult InteractiveTraceResult;
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.bDebugQuery = true;
	// FCollisionResponseParams CollisionResponseParams;

	UKismetSystemLibrary::LineTraceSingle(GetWorld()
		, CameraLocation
		, CameraTraceEndLocation
		, InteractiveTraceType
		, false
		, TArray<AActor*>()
		, EDrawDebugTrace::Persistent
		, InteractiveTraceResult
		, true
		, FLinearColor::Green
		, FLinearColor::Red
		, 5.f);

	if (InteractiveTraceResult.bBlockingHit && InteractiveTraceResult.GetActor())
	{
		// bool bIsImplemented = InteractiveTraceResult.GetActor()->GetClass()->ImplementsInterface(UInteractiveInterface::StaticClass());
		bool bIsImplemented = InteractiveTraceResult.GetActor()->Implements<UInteractiveInterface>();

		if (bIsImplemented)
		{
			IInteractiveInterface* InteractiveActor = Cast<IInteractiveInterface>(InteractiveTraceResult.GetActor());
			InteractiveActor->Interactive();
		}
	}

	// 2. 再进行重叠检测
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
	CombatCopmponent->PlayerCharacter = this;
	BackpackComponent->PlayerCharacter = this;
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
	
	HighLightPickableObject();
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
	
	if (GetCombatComponent())
	{
		GetCombatComponent()->SC_Aim(true);
	}
}

void ABlasterCharacter::AimButtonReleased()
{
	if (GetCombatComponent())
	{
		GetCombatComponent()->SC_Aim(false);
	}
}

void ABlasterCharacter::FireButtonPressed()
{
	if (GetCombatComponent())
	{
		if (HasAuthority())
		{
			GetCombatComponent()->SNC_Fire();
		}
		else
		{
			GetCombatComponent()->CC_Fire();
		}
	}
}

void ABlasterCharacter::FireHold()
{
	if (GetCombatComponent())
	{
		if (HasAuthority())
		{
		
			GetCombatComponent()->SNC_FireHold();
		}
		else
		{
			GetCombatComponent()->CC_FireHold();
		}
	}
	
}

void ABlasterCharacter::FireButtonReleased()
{
	if (GetCombatComponent())
	{
		if (HasAuthority())
		{
			GetCombatComponent()->SNC_FireHoldStop();
		}
		else
		{
			GetCombatComponent()->CC_FireHoldStop();
		}
	}
}

void ABlasterCharacter::ReloadButtonPressed()
{
	if (GetCombatComponent())
	{
		if (HasAuthority())
		{
			// 服务器作为客户端
			GetCombatComponent()->SNC_Reload();
		}
		else
		{
			// 客户端
			GetCombatComponent()->CC_Reload();
		}
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
		if (BackpackComponent->GetBackpackWidget())
		{
			BackpackComponent->GetBackpackWidget()->RotateDragItemWidget();
		}
	}
}

void ABlasterCharacter::Pickup()
{

	// 如果本地客户端作为服务器，需要进行射线检测后才能执行拾取，但不需要进行客户端模拟阶段
	
	if (HasAuthority())
	{
		AItemBase* PickupItem = TracePickableObject(EPickableObjectState::Pickup);
		if (PickupItem && PickableObjects.Contains(PickupItem))
		{
			SC_Pickup(PickupItem, GetWorld()->TimeSeconds);
		}
	}

	// 不允许进行客户端拾取，统一在服务器拾取后得到有保证的数据。
	else
	{
		CC_Pickup();
	}
}

void ABlasterCharacter::CC_Pickup_Implementation()
{
	// 客户端射线检测是否有可拾取物品
	AItemBase* PickupItem = TracePickableObject(EPickableObjectState::Pickup);

	if (PickupItem)
	{
		// 客户端模拟拾取
		FPickupResult PickupResult = GetBackpackComponent()->Pickup(PickupItem);
		
		// 客户端拾取成功后，服务器执行实际拾取
		float BackpackItemChangedTime= GetWorld()->GetTimeSeconds();
		
		if (PickupResult.Result == EPickupResult::AddNewItem_All
			|| PickupResult.Result == EPickupResult::StackAdd_All)
		{
			SC_Pickup(PickupItem, BackpackItemChangedTime);
			// 客户端隐藏物品，等待服务器拾取成功后销毁
			PickupItem->PutInBackpackHandle();
			
			GetBackpackComponent()->OnClientBackpackItemChanged.Broadcast(BackpackItemChangedTime);
		}
		else if(PickupResult.Result == EPickupResult::AddNewItem_Part
			|| PickupResult.Result == EPickupResult::StackAdd_Part)
		{
			// 部分拾取不需要客户端隐藏物品
			SC_Pickup(PickupItem, BackpackItemChangedTime);
			GetBackpackComponent()->OnClientBackpackItemChanged.Broadcast(BackpackItemChangedTime);
		}
	}
}

void ABlasterCharacter::SC_Pickup_Implementation(AItemBase* PickupedUpItem, float BackpackItemChangedTime)
{
	// 客户端触发可拾取后，在服务器中只需要附近有该可拾取的物品就可直接进行拾取，而不用向客户端需要先进行射线检测再拾取
	bHasPickableObject = false;
	for (auto PickableObject : PickableObjects)
	{
		if (PickableObject == PickupedUpItem)
		{
			GetBackpackComponent()->SC_Pickup(PickableObject, BackpackItemChangedTime);

			bHasPickableObject = true;
			break;
		}
	}

	// 客户端检测到有可拾取物品，但在服务器进行检测时由于网络问题发现可拾取物品已经不存在，那么将直接拾取失败
	if (!bHasPickableObject)
	{
		if (BackpackLagCompensationComponent)
		{
			// 反馈失败，处理拾取的 AItemBase
			BackpackLagCompensationComponent->ServerFeedbackPickupItemFailture(PickupedUpItem);
		}
		if (BackpackLagCompensationComponent)
		{
			// 反馈失败，处理客户端的背包数据
			BackpackLagCompensationComponent->ServerFeedbackBackpackItemChangedResult(BackpackItemChangedTime);
		}
	}
}

void ABlasterCharacter::EquipButtonPressed()
{
	AItemBase* PickupItem = TracePickableObject(EPickableObjectState::Pickup);
	if (PickupItem)
	{
		float EquitTime = GetWorld()->GetTimeSeconds();
		GetCombatComponent()->SC_Equipment(PickupItem, EquitTime);
	}
}

void ABlasterCharacter::UnEquipButtonPressed()
{
	if (HasAuthority())
	{
		GetCombatComponent()->SNC_UnEquipment();
	}
	else
	{
		GetCombatComponent()->CC_UnEquipment();
	}
}

void ABlasterCharacter::HideCameraIfCharacterClose()
{
	if (!IsLocallyControlled())
	{
		return;
	}

	// if ((PlayerCameraComponent->GetCameraComponent()->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold)
	// {
	// 	GetMesh()->SetVisibility(false);
	// 	// if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
	// 	// {
	// 	// 	// 仅对拥有者不可见
	// 	// 	Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
	// 	// }
	// }
	// else
	// {
	// 	GetMesh()->SetVisibility(true);
	// 	// if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
	// 	// {
	// 	// 	// 仅对拥有者不可见
	// 	// 	Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
	// 	// }
	// }
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

	PlayerCameraComponent->GetCameraComponent()->bUsePawnControlRotation = false;
	PlayerCameraComponent->GetSpringArmComponent()->bUsePawnControlRotation = true;

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
	if (!HasAuthority())
	{
		return;
	}
	
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

void ABlasterCharacter::HighLightPickableObject()
{
	if (bHasPickableObject)
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		if (ensure(PlayerController))
		{
			FVector2D ViewportSize;

			if (GEngine && GEngine->GameViewport)
			{
				GEngine->GameViewport->GetViewportSize(ViewportSize);
			}

			FVector2D CrosshairLocation(ViewportSize.X / 2.0f, ViewportSize.Y / 2.0f);
			
			FVector WorldLocation;
			FVector WorldDirection;

			
			PlayerController->DeprojectScreenPositionToWorld(CrosshairLocation.X, CrosshairLocation.Y, WorldLocation, WorldDirection);

			// TODO 为什么在客户端的调用的 RPC 为什么得到的投影坐标和方向都是 0 ？
			if (WorldDirection.Size() == 0)
			{
				WorldLocation = GetFollowCamera()->GetComponentLocation();

				WorldDirection = PlayerCameraComponent->GetSpringArmComponent()->GetComponentLocation() - WorldLocation;
				WorldDirection.Normalize();
			}
			
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
				// 
			}
		}
	}
}

AItemBase* ABlasterCharacter::TracePickableObject(EPickableObjectState PickableObjectState)
{
	if (PickableObjects.Num() > 0)
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetController());

		if (PlayerController)
		{
			FVector2D ViewportSize;
			GEngine->GameViewport->GetViewportSize(ViewportSize);

			FVector2D CrosshairLocation(ViewportSize.X / 2.0f, ViewportSize.Y / 2.0f);
			
			FVector WorldLocation;
			FVector WorldDirection;

			
			PlayerController->DeprojectScreenPositionToWorld(CrosshairLocation.X, CrosshairLocation.Y, WorldLocation, WorldDirection);

			if (WorldDirection.Size() == 0)
			{
				WorldLocation = GetFollowCamera()->GetComponentLocation();

				WorldDirection = PlayerCameraComponent->GetSpringArmComponent()->GetComponentLocation() - WorldLocation;
				WorldDirection.Normalize();
			}
			
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

			const bool HitBlock = HitResult.bBlockingHit;

			if (HitBlock)
			{
				return Cast<AItemBase>(HitResult.GetActor());
			}
			else
			{
				return nullptr;
			}
			// if (HitBlock)
			// {
			// 	PickableObjectData.PickableActor = HitResult.GetActor();
			// 	PickableObjectData.HandleState = EPickableObjectState::Default;
			// 	PickableObjectData.TargetState = PickableObjectState;
			// }
			// else
			// {
			// 	PickableObjectData.PickableActor = nullptr;
			// 	PickableObjectData.HandleState = EPickableObjectState::Default;
			// 	PickableObjectData.TargetState = EPickableObjectState::Default;
			// }
		}
	}
	return nullptr;
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

// TODO 武器显示控件
void ABlasterCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	// if (!Weapon && OverlappingWeapon)
	// {
	// 	// OverlappingWeapon->ShowPickupWidget(false);
	// }
	//
	// OverlappingWeapon = Weapon;
	// // 是否是本地玩家控制
	// if (IsLocallyControlled())
	// {
	// 	if (OverlappingWeapon)
	// 	{
	// 		// OverlappingWeapon->ShowPickupWidget(true);
	// 	}
	// }
}

bool ABlasterCharacter::IsWeaponEquipped()
{
	return (GetCombatComponent() && GetCombatComponent()->EquippedWeapon);
}

bool ABlasterCharacter::IsAiming()
{
	return (GetCombatComponent() && GetCombatComponent()->bIsAiming);
}

AWeapon* ABlasterCharacter::GetEquippedWeapon()
{
	if (GetCombatComponent() == nullptr)
	{
		return nullptr;
	}

	return GetCombatComponent()->EquippedWeapon;
}

// TODO 射击命中目标
FVector ABlasterCharacter::GetHitTarget() const
{
	if (GetCombatComponent() == nullptr)
	{
		return FVector();
	}

	//return Combat->HitTarget;
	return FVector();
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
	// PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ABlasterCharacter::Jump);
	// PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &ABlasterCharacter::EquipButtonPressed);
	// PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ABlasterCharacter::CrouchButtonPressed);
	// PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ABlasterCharacter::AimButtonPressed);
	// PlayerInputComponent->BindAction("Aim", IE_Released, this, &ABlasterCharacter::AimButtonReleased);
	//
	// PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ABlasterCharacter::FireButtonPressed);
	// PlayerInputComponent->BindAction("Fire", IE_Released, this, &ABlasterCharacter::FireButtonReleased);
	// PlayerInputComponent->BindAction("Reload", IE_Released, this, &ABlasterCharacter::ReloadButtonPressed);

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
		if (EIA_Aim)
		{
			EInputComponent->BindAction(EIA_Aim, ETriggerEvent::Triggered, this, &ThisClass::AimButtonPressed);
		}
		if (EIA_UnAim)
		{
			EInputComponent->BindAction(EIA_UnAim, ETriggerEvent::Triggered, this, &ThisClass::AimButtonReleased);
		}

		if (EIA_Crouch)
		{
			EInputComponent->BindAction(EIA_Crouch, ETriggerEvent::Triggered, this, &ThisClass::CrouchButtonPressed);
		}

		if (EIA_Discard)
		{
			// TODO 装备丢弃
		}

		if (EIA_Pickup)
		{
			EInputComponent->BindAction(EIA_Pickup, ETriggerEvent::Triggered, this, &ThisClass::Pickup);
		}

		if (EIA_Equipment)
		{
			EInputComponent->BindAction(EIA_Equipment, ETriggerEvent::Triggered, this, &ABlasterCharacter::EquipButtonPressed);
		}

		if (EIA_UnEquipment)
		{
			EInputComponent->BindAction(EIA_UnEquipment, ETriggerEvent::Triggered, this, &ABlasterCharacter::UnEquipButtonPressed);
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
			EInputComponent->BindAction(EIA_Shoot, ETriggerEvent::Triggered, this, &ThisClass::FireButtonPressed);
		}

		if (EIA_ShootHold)
		{
			EInputComponent->BindAction(EIA_ShootHold, ETriggerEvent::Triggered, this, &ThisClass::FireHold);
		}

		if (EIA_ShootButtonReleased)
		{
			EInputComponent->BindAction(EIA_ShootButtonReleased, ETriggerEvent::Triggered, this, &ThisClass::FireButtonReleased);
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
			EInputComponent->BindAction(EIA_ReloadAmmo, ETriggerEvent::Triggered, this, &ThisClass::ReloadButtonPressed);
		}

		if (EIA_UseSwitch)
		{
			// TODO 武器切换
		}

		if (EIA_OpenOrCloseBackpack)
		{
			EInputComponent->BindAction(EIA_OpenOrCloseBackpack, ETriggerEvent::Triggered, this, &ThisClass::OpenOrCloseBackpack);
		}

		if (EIA_RotateDragItemWidget)
		{
			EInputComponent->BindAction(EIA_RotateDragItemWidget, ETriggerEvent::Triggered, this, &ThisClass::RotateDragItemWidget);
		}

		if (EIA_Interactive)
		{
			EInputComponent->BindAction(EIA_Interactive, ETriggerEvent::Triggered, this, &ThisClass::Interactive);
		}

		if (EIA_OpenCamera)
		{
			EInputComponent->BindAction(EIA_OpenCamera, ETriggerEvent::Triggered, PlayerCameraComponent, &UUnrealCameraComponent::OpenCamera);
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
	// if (Combat && Combat->EquippedWeapon)
	// {
	// 	// 如果死亡时装备武器，将丢弃武器
	// 	// Combat->EquippedWeapon->Dropped();
	// }
	
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
	// if (Combat == nullptr)
	// {
	// 	return ECombatState::ECS_MAX;
	// }
	//
	// return Combat->CombatState;
	return ECombatState::Combat;
}

// extern TAutoConsoleVariable<int32> CVarShowLocalRoleDebugInfo;

void ABlasterCharacter::DisplayRole(ENetRole Role)
{
	{
		float bIsShow = AConsoleVariableActor::GetShowLocalRoleDebugInfo();
		if (bIsShow)
		{
			switch (Role)
			{
			case ROLE_Authority:
				{
					UE_LOG(LogTemp, Warning, TEXT("ROLE_Authority"));
					break;
				}
			case ROLE_AutonomousProxy:
				{
					UE_LOG(LogTemp, Warning, TEXT("ROLE_AutonomousProxy"));
					break;
				}
			case ROLE_SimulatedProxy:
				{
					UE_LOG(LogTemp, Warning, TEXT("ROLE_SimulatedProxy"));
					break;
				}
			case ROLE_None:
				{
					UE_LOG(LogTemp, Warning, TEXT("ROLE_None"));
					break;
				}
			case ROLE_MAX:
				{
					UE_LOG(LogTemp, Warning, TEXT("ROLE_MAX"));
					break;
				}
			default:
				{
					UE_LOG(LogTemp, Warning, TEXT("Default Role Output"));
					break;
				}
			}
		}
	}
}