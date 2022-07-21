// Fill out your copyright notice in the Description page of Project Settings.


#include "Unreal2DCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "PaperFlipbookComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "UnrealGame/Component/2D/Ureal2DClimbableComponent.h"
#include "UnrealGame/SpriteActor/ClimbableSpriteActor.h"

AUnreal2DCharacter::AUnreal2DCharacter(const FObjectInitializer& ObjectInitializer)
{
	//SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	//CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));

	//SpringArmComponent->SetupAttachment(RootComponent);

	//CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	ClimbableComponent = CreateDefaultSubobject<UUreal2DClimbableComponent>(TEXT("ClimbableComponent"));
	
	ClimbableComponent->SetIsReplicated(true);

	GetSprite()->SetIsReplicated(true);
}

void AUnreal2DCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (HasAuthority())
	{
		UpdateFlipBookState(DeltaSeconds);
	}
	
	FlipBookStateMachine();
	TemporaryAnimationPlayEndHandle();
}

void AUnreal2DCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem)
		{
			Subsystem->ClearAllMappings();

			UE_LOG(LogTemp, Warning, TEXT("Character Mapping Setting"));
			Subsystem->AddMappingContext(Unreal2DGameInputMappingContext, 0);
		}
	}

	UEnhancedInputComponent* EInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EInputComponent)
	{
		if (EIA_MoveForward)
		{
			EInputComponent->BindAction(EIA_MoveForward, ETriggerEvent::Triggered, this, &ThisClass::GroundMovement);
		}

		if (EIA_MoveBackward)
		{
			EInputComponent->BindAction(EIA_MoveBackward, ETriggerEvent::Triggered, this, &ThisClass::GroundMovement);
		}

		if (EIA_Crouch)
		{
			EInputComponent->BindAction(EIA_Crouch, ETriggerEvent::Triggered, this, &ThisClass::CrouchButtomPressed);
		}

		if (EIA_UnCrouch)
		{
			EInputComponent->BindAction(EIA_UnCrouch, ETriggerEvent::Triggered, this, &ThisClass::CrouchButtomReleased);
		}
		
		if (EIA_ClimbUp)
		{
			EInputComponent->BindAction(EIA_ClimbUp, ETriggerEvent::Triggered, this, &ThisClass::ClimbUpButtomPressed);
		}
		
		if (EIA_ClimbDown)
		{
			EInputComponent->BindAction(EIA_ClimbDown, ETriggerEvent::Triggered, this, &ThisClass::ClimbDownButtomPressed);
		}

		if (EIA_Jump)
		{
			EInputComponent->BindAction(EIA_Jump, ETriggerEvent::Triggered, this, &ThisClass::JumpButtomPressed);
		}

		if (EIA_Attack)
		{
			EInputComponent->BindAction(EIA_Attack, ETriggerEvent::Triggered, this, &ThisClass::AttackButtomPressed);
		}

		if (EIA_Dash)
		{
			EInputComponent->BindAction(EIA_Dash, ETriggerEvent::Triggered, this, &ThisClass::DashButtomPressed);
		}

		if (EIA_Slide)
		{
			EInputComponent->BindAction(EIA_Slide, ETriggerEvent::Triggered, this, &ThisClass::SlideButtomPressed);
		}

		// 跳跃组件绑定
		if (ClimbableComponent)
		{
			ClimbableComponent->InitPlayerInputComponent();
		}
	}
}

void AUnreal2DCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AUnreal2DCharacter, FlipBookState);
	DOREPLIFETIME(AUnreal2DCharacter, FlipBookTemporaryAnimationState);
	DOREPLIFETIME(AUnreal2DCharacter, PreCharacterLocation);
	DOREPLIFETIME(AUnreal2DCharacter, JumpToFallinginterval);
	DOREPLIFETIME(AUnreal2DCharacter, FallingTime);
	DOREPLIFETIME(AUnreal2DCharacter, CameraActor);
}

void AUnreal2DCharacter::SetCameraActor_Implementation(AUnreal2DCameraActor* InCameraActor)
{
	CameraActor = InCameraActor;

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{

		AActor* TempCameraActor = Cast<AActor>(CameraActor);
		PlayerController->SetViewTarget(TempCameraActor);
	}
}

void AUnreal2DCharacter::BeginDestroy()
{
	Super::BeginDestroy();
}


void AUnreal2DCharacter::SC_SetSpriteRotation_Implementation(FRotator SpriteRotator)
{
	SetSpriteRotation(SpriteRotator);
}

void AUnreal2DCharacter::SetSpriteRotation_Implementation(FRotator SpriteRotator)
{
	GetSprite()->SetWorldRotation(SpriteRotator);
}

void AUnreal2DCharacter::GroundMovement(const FInputActionValue& ActionValue)
{
	float Value = ActionValue.GetMagnitude();

	if (Value == 0)
	{
		return;
	}

	// 根据 Sprite 的 Yaw 来判断玩家的朝向
	USceneComponent* SpriteSceneComponent = Cast<USceneComponent>(GetSprite());
	float SpriteYaw = SpriteSceneComponent->K2_GetComponentRotation().Yaw;

	bool bIsSameDirection = SpriteYaw == 0 && Value > 0 || SpriteYaw == 180 && Value < 0;

	// TODO 如果在攀爬状态(藤蔓)、下落状态，可以转身，但不可移动
	if (FlipBookState == EFlipBookState::Falling
		|| (FlipBookState == EFlipBookState::Climbing && ClimbableComponent->ClimbingType == EClimbableType::Vine)
		|| FlipBookState == EFlipBookState::GroundMovement
		|| FlipBookState == EFlipBookState::GroundIdle)
	{
		if (!bIsSameDirection)
		{
			float Yaw = SpriteYaw == 180 ? 0 : 180;
			SC_SetSpriteRotation(FRotator(0, Yaw, 0));
		}
	}
	
	if (FlipBookState == EFlipBookState::GroundMovement
		|| FlipBookState == EFlipBookState::GroundIdle)
	{
		AddMovementInput(MoveForwarDirection, Value);
	}
	
}

void AUnreal2DCharacter::CrouchButtomPressed()
{
	/*
	 * 只有在地面时才可以蹲伏
	 */
	if (FlipBookState == EFlipBookState::GroundIdle
		|| FlipBookState == EFlipBookState::GroundMovement)
	{
		FlipBookState = EFlipBookState::TemporaryAnimation;
		FlipBookTemporaryAnimationState = EFlipBookTemporaryAnimation::GroundCrouch;
		GetSprite()->SetFlipbook(FB_Crouch);
	}
}

void AUnreal2DCharacter::CrouchButtomReleased()
{
	if (FlipBookState == EFlipBookState::TemporaryAnimation
		&& FlipBookTemporaryAnimationState == EFlipBookTemporaryAnimation::GroundCrouch)
	{
		FlipBookState = EFlipBookState::Transitioning;
	}
}

void AUnreal2DCharacter::ClimbUpButtomPressed(const FInputActionValue& ActionValue)
{
	if (ClimbableComponent->bIsCanClimb
		&& ClimbableComponent->CurrentClimbableActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Climb Up"));
		float PlayerZ = GetActorLocation().Z;
		float ClimbEndPositionZ = ClimbableComponent->CurrentClimbableActor->ClimbEndLocation.Z;
		if (ClimbEndPositionZ <= PlayerZ)
		{
			// 如果在可攀爬区域低于可攀爬起点，则不允许通过向上攀爬进入攀爬状态
			return;
		}
		ClimbableComponent->EnterClimbState();
	}
}

void AUnreal2DCharacter::ClimbDownButtomPressed(const FInputActionValue& ActionValue)
{
	// 可攀爬，且不可是处于攀爬下滑状态
	if (ClimbableComponent->bIsCanClimb
		&& ClimbableComponent->CurrentClimbableActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Climb Down"));
		
		float PlayerZ = GetActorLocation().Z;
		float ClimbEndPositionZ = ClimbableComponent->CurrentClimbableActor->ClimbStartLocation.Z;
		if (ClimbEndPositionZ >= PlayerZ)
		{
			// 如果在可攀爬区域低于可攀爬起点，则不允许通过向下攀爬进入攀爬状态
			return;
		}
		ClimbableComponent->EnterClimbState();
	}
}

void AUnreal2DCharacter::JumpButtomPressed()
{
	if (GetCharacterMovement()->IsFalling())
	{
		return;
	}
	FlipBookState = EFlipBookState::Jumping;
	Jump();
	// TODO 跳跃的其他处理
}

void AUnreal2DCharacter::AttackButtomPressed()
{
	// TODO 播放攻击动画

	// TODO 是否设计不可攻击状态?

	// TODO 攻击间隔，还是只有攻击播放结束后才能进行下一轮攻击
	if (FlipBookState == EFlipBookState::TemporaryAnimation
		&& FlipBookTemporaryAnimationState == EFlipBookTemporaryAnimation::Attack
		&& !IsCurrentFlipBookAnimationPlayEnd())
	{
		/*
		 * 当处于攻击状态时，如果当前攻击动画序列未播放完毕，不进行下轮攻击，不设计自动攻击
		 */
		return;
	}
	
	FlipBookState = EFlipBookState::TemporaryAnimation;

	FlipBookTemporaryAnimationState = EFlipBookTemporaryAnimation::Attack;

	GetSprite()->SetFlipbook(FB_Attack);
}

void AUnreal2DCharacter::DashButtomPressed()
{
	/*
	 * 1. 只有在地面上时才可以猛冲
	 */ 
	if (FlipBookState == EFlipBookState::GroundIdle
		|| FlipBookState == EFlipBookState::GroundMovement)
	{
		// 播放猛冲动画

		FlipBookState = EFlipBookState::TemporaryAnimation;

		FlipBookTemporaryAnimationState = EFlipBookTemporaryAnimation::GroundDash;

		GetSprite()->SetFlipbook(FB_Dash);
	}
}

void AUnreal2DCharacter::SlideButtomPressed()
{
	/*
	 * 只有在地面时才可以滑动
	 */
	if (FlipBookState == EFlipBookState::GroundIdle
		|| FlipBookState == EFlipBookState::GroundMovement
		|| (FlipBookState == EFlipBookState::TemporaryAnimation && FlipBookTemporaryAnimationState == EFlipBookTemporaryAnimation::GroundCrouch))
	{
		// 播放滑动动画
		FlipBookState = EFlipBookState::TemporaryAnimation;

		FlipBookTemporaryAnimationState = EFlipBookTemporaryAnimation::Slide;

		GetSprite()->SetFlipbook(FB_Slide);
	}
	
}

void AUnreal2DCharacter::UpdateFlipBookState(float DeltaTime)
{
	if (FlipBookState == EFlipBookState::TemporaryAnimation)
	{
		// 等待临时动画播放结束。
		return;
	}
	else
	{
		bool IsFalling = GetCharacterMovement()->IsFalling();
	
		UCharacterMovementComponent* TempCharacterMovementComponent = GetCharacterMovement();

		if (TempCharacterMovementComponent->GetCurrentAcceleration().Size() == 0
			&& PreCharacterLocation == GetActorLocation()
			&& FlipBookState != EFlipBookState::Climbing
			&& !IsFalling)
		{
			/**
			 * 地面空闲状态进入条件：
			 * 1. 没有用户输入 - TempCharacterMovementComponent->GetCurrentAcceleration().Size()，当没有输入时，不会计算加速度，代表加速度为 0，但是玩家可能在下落
			 * 2. 上一个位置与当前位置不变（静止）
			 * 3. 不处于攀爬状态
			 * 4. 不处于下落状态
			 */
			FlipBookState = EFlipBookState::GroundIdle;
		}
		else if (TempCharacterMovementComponent->GetCurrentAcceleration().Size() > 0
			&& !IsFalling
			&& FlipBookState != EFlipBookState::Climbing
			)
		{
			/*
			 * 地面移动状态进入条件：
			 *
			 * 1. 具有用户输入
			 * 2. 没有处于下落状态
			 * 3. 不处于攀爬状态
			 */
			FlipBookState = EFlipBookState::GroundMovement;

		}
		else if (bWasJumping && FallingTime == 0)
		{
			// 当 FallingTime 不等于 0 时，说明已经设置过一次了，不需要重复设置。
			
			/*
			 * 当跳跃时，只会触发一次 bWasJumping = true，然后自动设置 bWasJumping = fasle；
			 * 因此当检测到 bWasJumping 为 true 就设置为跳跃状态，跳跃到下落的过渡由后续逻辑处理。
			 */
			FlipBookState = EFlipBookState::Jumping;
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
				FallingTime = 0;
				FlipBookState = EFlipBookState::Falling;
				// 当下落时将 bWasJumping 设为 false，为了恢复处理攀爬时跳跃设置的 bWasJumping = true。
				bWasJumping = false;
			}
		}
	
		PreCharacterLocation = GetActorLocation();
	}
	
	
}

void AUnreal2DCharacter::FlipBookStateMachine()
{
	UPaperFlipbookComponent* FlipbookComponent = GetSprite();
	switch (FlipBookState)
	{
		case EFlipBookState::GroundIdle:
			{
				FlipbookComponent->SetFlipbook(FB_Idle);
				break;
			}
		case EFlipBookState::GroundMovement:
				{
					FlipbookComponent->SetFlipbook(FB_Run);
					break;
				}
		case EFlipBookState::Jumping:
			{
				FlipbookComponent->SetFlipbook(FB_Jump);
				break;
			}
		case EFlipBookState::Falling:
			{
				FlipbookComponent->SetFlipbook(FB_Falling);
				break;
			}
		case EFlipBookState::Climbing:
			{
				ClimbableComponent->FlipBookClimbingStateMachine();
				break;
			}

		case EFlipBookState::TemporaryAnimation:
			{
				
			}
		default:
			{
				
			}
	}
}

bool AUnreal2DCharacter::IsCurrentFlipBookAnimationPlayEnd()
{
	int32 FlipBookAnimationFrameLength = GetSprite()->GetFlipbookLengthInFrames();
	int32 FlipBookAnimationCurrentFramePosition = GetSprite()->GetPlaybackPositionInFrames();
	// GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green,
	// 	FString::Printf(TEXT("CurFrame:{%d}, FrameNums:{%d}"), FlipBookAnimationFrameLength, FlipBookAnimationCurrentFramePosition));
	return FlipBookAnimationCurrentFramePosition == FlipBookAnimationFrameLength - 1;
}

void AUnreal2DCharacter::TemporaryAnimationPlayEndHandle()
{
	if (FlipBookState != EFlipBookState::TemporaryAnimation)
	{
		return;
	}
	
	bool IsCurrentAnimationPlayEnd = IsCurrentFlipBookAnimationPlayEnd();

	if (!IsCurrentAnimationPlayEnd)
	{
		return;
	}
	
	UPaperFlipbookComponent* FlipbookComponent = GetSprite();
	
	switch (FlipBookTemporaryAnimationState)
	{
	case EFlipBookTemporaryAnimation::GroundCrouch:
		{
			FlipbookComponent->SetFlipbook(FB_CrouchPose);
			break;
		}
	case EFlipBookTemporaryAnimation::GroundDash:
		{
			FlipBookState = EFlipBookState::Transitioning;
			break;
		}
	case EFlipBookTemporaryAnimation::GroundDashAttack:
		{
			FlipBookState = EFlipBookState::Transitioning;
			break;
		}
	case EFlipBookTemporaryAnimation::Death:
		{
			FlipbookComponent->SetFlipbook(FB_DeathPose);
			break;
		}
	case EFlipBookTemporaryAnimation::EdgeGrab:
		{
			FlipBookState = EFlipBookState::Transitioning;
			break;
		}
	case EFlipBookTemporaryAnimation::Hurted:
		{
			FlipBookState = EFlipBookState::Transitioning;
			break;
		}
	case EFlipBookTemporaryAnimation::Slide:
		{
			FlipBookState = EFlipBookState::Transitioning;
			break;
		}
	case EFlipBookTemporaryAnimation::Attack:
		{
			FlipBookState = EFlipBookState::Transitioning;
			break;
		}
	default:
		{
					
		}
	}
}

void AUnreal2DCharacter::SetFlipBookState_Implementation(EFlipBookState InFlipBookState)
{
	this->FlipBookState = InFlipBookState;
}
