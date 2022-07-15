// Fill out your copyright notice in the Description page of Project Settings.


#include "Unreal2DCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "PaperFlipbookComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"

AUnreal2DCharacter::AUnreal2DCharacter(const FObjectInitializer& ObjectInitializer)
{
	//SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	//CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));

	//SpringArmComponent->SetupAttachment(RootComponent);

	//CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
}

void AUnreal2DCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (HasAuthority())
	{
		UpdateMovementState(DeltaSeconds);
	}
	
	MovementStateMachine();

	// TODO
	// 1. 更新运动状态

	// 2. 根据运动状态设置对应的 Flipbook
}

void AUnreal2DCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();
			Subsystem->AddMappingContext(Unreal2DGameInputMappingContext, 0);
		}
	}

	if (UEnhancedInputComponent* EInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (EIA_MoveForward)
		{
			EInputComponent->BindAction(EIA_MoveForward, ETriggerEvent::Triggered, this, &ThisClass::MoveForward);
		}

		if (EIA_Climp)
		{
			EInputComponent->BindAction(EIA_Climp, ETriggerEvent::Triggered, this, &ThisClass::Climb);
		}

		if (EIA_Jump)
		{
			EInputComponent->BindAction(EIA_Jump, ETriggerEvent::Triggered, this, &ThisClass::JumpButtomPressed);
		}
	}
}

void AUnreal2DCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AUnreal2DCharacter, MovementState);
}

void AUnreal2DCharacter::SetCameraActor(AUnreal2DCameraActor* InCameraActor)
{
	CameraActor = InCameraActor;

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{

		AActor* TempCameraActor = Cast<AActor>(CameraActor);
		PlayerController->SetViewTarget(TempCameraActor);
	}
}

void AUnreal2DCharacter::MoveForward(const FInputActionValue& ActionValue)
{
	float Value = ActionValue.GetMagnitude();

	if (Value == 0)
	{
		return;
	}

	// 根据 Sprite 的 Yaw 来判断玩家的朝向
	USceneComponent* SpriteSceneComponent = Cast<USceneComponent>(GetSprite());
	float SpriteYaw = SpriteSceneComponent->K2_GetComponentRotation().Yaw;

	bool bIsSameDirection = SpriteYaw == 0 && Value > 0
		|| SpriteYaw == 180 && Value < 0;

	if (!bIsSameDirection)
	{
		float Yaw = SpriteYaw == 180 ? 0 : 180;
		SpriteSceneComponent->SetWorldRotation(FRotator(0, Yaw, 0));
	}

	// TODO 如果在攀爬状态、下落状态，可以转身，但不可移动
	AddMovementInput(MoveForwarDirection, Value);
}

void AUnreal2DCharacter::Climb(const FInputActionValue& ActionValue)
{
	MovementState = EMovementState::Climbing;
}

void AUnreal2DCharacter::JumpButtomPressed()
{
	Jump();
	// TODO 跳跃的其他处理
}

void AUnreal2DCharacter::UpdateMovementState(float DeltaTime)
{
	bool IsFalling = GetCharacterMovement()->IsFalling();
	
	UCharacterMovementComponent* TempCharacterMovementComponent = GetCharacterMovement();
	// 当没有输入时，不会计算加速度，代表加速度为 0，但是玩家可能在下落
	if (TempCharacterMovementComponent->GetCurrentAcceleration().Size() == 0
		&& PreCharacterLocation == GetActorLocation()
		&& MovementState != EMovementState::Climbing
		&& !IsFalling)
	{
		MovementState = EMovementState::Idle;
	}
	else if (TempCharacterMovementComponent->GetCurrentAcceleration().Size() > 0
		&& !IsFalling
		&& MovementState != EMovementState::Climbing
		)
	{
		MovementState = EMovementState::GroundMovement;

	}
	else if (bWasJumping)
	{
		MovementState = EMovementState::Jumping;
		// 当处于 Falling 下落状态时，如果是跳跃导致的下落，那么等到一定的时间后才变成下落状态
		FallingTime = JumpToFallinginterval;
	}

	// 下落状态判断
	if (IsFalling)
	{
		if (FallingTime > 0)
		{
			FallingTime -= DeltaTime;
		}
		else
		{
			MovementState = EMovementState::Falling;
		}
	}
	
	PreCharacterLocation = GetActorLocation();

	
	// 当跳跃时会，在"角色移动组件"中会处于 "Falling" 状态，一般当 Falling 足够时间后才真正确认为"Falling(下落状态)"
	// bool IsFalling = GetCharacterMovement()->IsFalling();
	//
	// // TODO 使用定时器，当 Falling 时定时，当不 Falling 时取消定时器，当运动状态改变时，一定要取消定时器
	//
	//
	// if (!IsFalling)
	// {
	// 	MovementState = EMovementState::GroundMovement;
	// }
}

void AUnreal2DCharacter::MovementStateMachine()
{
	UPaperFlipbookComponent* FlipbookComponent = GetSprite();
	switch (MovementState)
	{
	case EMovementState::GroundMovement:
		{
			FlipbookComponent->SetFlipbook(FB_Run);
			break;
		}
	case EMovementState::Jumping:
		{
			FlipbookComponent->SetFlipbook(FB_Jump);
			break;
		}
	case EMovementState::Falling:
		{
			FlipbookComponent->SetFlipbook(FB_Falling);
			break;
		}
	case EMovementState::Climbing:
		{
			FlipbookComponent->SetFlipbook(FB_Climb);
			break;
		}
	default:
		{
			FlipbookComponent->SetFlipbook(FB_Idle);
			break;
		}
	}
}
