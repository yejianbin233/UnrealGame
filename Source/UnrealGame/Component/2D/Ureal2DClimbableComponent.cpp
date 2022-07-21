// Fill out your copyright notice in the Description page of Project Settings.


#include "Ureal2DClimbableComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PaperFlipbookComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "UnrealGame/Character/Unreal2DCharacter.h"
#include "UnrealGame/SpriteActor/ClimbableSpriteActor.h"

// Sets default values for this component's properties
UUreal2DClimbableComponent::UUreal2DClimbableComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}


void UUreal2DClimbableComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UUreal2DClimbableComponent, ClimbingType);
	DOREPLIFETIME(UUreal2DClimbableComponent, ClimbingState);
	DOREPLIFETIME(UUreal2DClimbableComponent, bIsCanClimb);
	DOREPLIFETIME(UUreal2DClimbableComponent, CurrentClimbableActor);
	DOREPLIFETIME(UUreal2DClimbableComponent, ClimbRate);
	DOREPLIFETIME(UUreal2DClimbableComponent, ClimbSpeed);
	DOREPLIFETIME(UUreal2DClimbableComponent, XImpulseForceValue);
	DOREPLIFETIME(UUreal2DClimbableComponent, ZImpulseForceValue);
	DOREPLIFETIME(UUreal2DClimbableComponent, SecondDirectionRatio);
	DOREPLIFETIME(UUreal2DClimbableComponent, LastUpdatePlayerActorLocation);
}

void UUreal2DClimbableComponent::EnterClimbState_Implementation()
{
	// 1. 设置攀爬移动模式，手动设置攀爬的位置
	GetOwnerPlayerCharacter()->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	// 2. 设置玩家角色的 FlipBookState 为攀爬状态
	GetOwnerPlayerCharacter()->SetFlipBookState(EFlipBookState::Climbing);

	// 3. 切换玩家攀爬输入(增强输入)
	if (APlayerController* PlayerController = Cast<APlayerController>(GetOwnerPlayerCharacter()->GetController()))
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem)
		{
			Subsystem->ClearAllMappings();

			Subsystem->AddMappingContext(Unreal2DClimbInputMappingContext, 0);
		}
	}

	// 获取一个最近的攀爬位置
	// 默认处于空闲状态
	ClimbingState = EFlipBookClimbingState::Idle;

	if (CurrentClimbableActor->ClimbableType == EClimbableType::Wall)
	{
		ClimbingType = EClimbableType::Wall;
	}
	else if(CurrentClimbableActor->ClimbableType == EClimbableType::Vine)
	{
		ClimbingType = EClimbableType::Vine;
	}
	else if(CurrentClimbableActor->ClimbableType == EClimbableType::Ladder)
	{
		ClimbingType = EClimbableType::Ladder;
	}
	
	FVector ClimbLocation = CurrentClimbableActor->GetRecentClimbLocation(GetOwnerPlayerCharacter()->GetActorLocation());
	GetOwnerPlayerCharacter()->SetActorLocation(ClimbLocation);
}

void UUreal2DClimbableComponent::EndClimbState_Implementation()
{
	// 1. 退出攀爬移动模式，恢复正常的移动模式
	EndClimbMode();

	// 2. 设置玩家角色的 FlipBookState，使其可从攀爬过渡到其他状态
	GetOwnerPlayerCharacter()->SetFlipBookState(EFlipBookState::Transitioning);

	// 3. 切换玩家正常输入(增强输入)
	if (APlayerController* PlayerController = Cast<APlayerController>(GetOwnerPlayerCharacter()->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();
			Subsystem->AddMappingContext(GetOwnerPlayerCharacter()->Unreal2DGameInputMappingContext, 0);
		}
	}
}

void UUreal2DClimbableComponent::EnterOrLeaveClimbableArea_Implementation(AClimbableSpriteActor* ClimbableSpriteActor, bool IsEnter)
{
	if (IsEnter)
	{
		bIsCanClimb = true;
		CurrentClimbableActor = ClimbableSpriteActor;
	}
	else
	{
		bIsCanClimb = false;
		CurrentClimbableActor = nullptr;
	}
}

void UUreal2DClimbableComponent::ReleaseLeft()
{
	LeftRelease = true;
}

void UUreal2DClimbableComponent::ReleaseRight()
{
	RightRelease = true;
}

void UUreal2DClimbableComponent::ReleaseUp()
{
	UpRelease = true;
}

void UUreal2DClimbableComponent::ReleaseDown()
{
	DownRelease = true;
}

// Called when the game starts
void UUreal2DClimbableComponent::BeginPlay()
{
	Super::BeginPlay();
	// ...
	LeftRelease = true;
	RightRelease = true;
	UpRelease = true;
	DownRelease = true;
}

// Called every frame
void UUreal2DClimbableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetOwnerPlayerCharacter()->FlipBookState != EFlipBookState::Climbing)
	{
		return;
	}

	
	float LastZ = LastUpdatePlayerActorLocation.Z;
	float CurrentZ = GetOwnerPlayerCharacter()->GetActorLocation().Z;
	
	if (LastZ == CurrentZ)
	{
		ClimbingState = EFlipBookClimbingState::Idle;
	}
	// else
	// {
	// 	ClimbingState = EFlipBookClimbingState::Climbing;
	// }
	
	if (GetOwnerPlayerCharacter()->FlipBookState == EFlipBookState::Climbing)
	{
		FlipBookClimbingStateMachine();
	}

	LastUpdatePlayerActorLocation = GetOwnerPlayerCharacter()->GetActorLocation();
	// ...
}

void UUreal2DClimbableComponent::InitPlayerInputComponent()
{
	UEnhancedInputComponent* EInputComponent = Cast<UEnhancedInputComponent>(GetOwnerPlayerCharacter()->InputComponent);
	if (EInputComponent)
	{
		if (EInputComponent)
		{
			if (EIA_MoveLeft)
			{
				EInputComponent->BindAction(EIA_MoveLeft, ETriggerEvent::Triggered, this, &UUreal2DClimbableComponent::ClimbLeftMovement);
			}
			if (EIA_MoveRight)
			{
				EInputComponent->BindAction(EIA_MoveRight, ETriggerEvent::Triggered, this, &UUreal2DClimbableComponent::ClimbRightMovement);
			}

			if (EIA_MoveUp)
			{
				EInputComponent->BindAction(EIA_MoveUp, ETriggerEvent::Triggered, this, &UUreal2DClimbableComponent::ClimbUpMovement);
			}

			if (EIA_MoveDown)
			{
				EInputComponent->BindAction(EIA_MoveDown, ETriggerEvent::Triggered, this, &UUreal2DClimbableComponent::ClimbDownMovement);
			}

			if (EIA_ClimbToJump)
			{
				EInputComponent->BindAction(EIA_ClimbToJump, ETriggerEvent::Triggered, this, &UUreal2DClimbableComponent::ClimbToJump);
			}

			if (EIA_ReleaseLeft)
			{
				EInputComponent->BindAction(EIA_ReleaseLeft, ETriggerEvent::Triggered, this, &UUreal2DClimbableComponent::ReleaseLeft);
			}

			if (EIA_ReleaseRight)
			{
				EInputComponent->BindAction(EIA_ReleaseRight, ETriggerEvent::Triggered, this, &UUreal2DClimbableComponent::ReleaseRight);
			}

			if (EIA_ReleaseUp)
			{
				EInputComponent->BindAction(EIA_ReleaseUp, ETriggerEvent::Triggered, this, &UUreal2DClimbableComponent::ReleaseUp);
			}

			if (EIA_ReleaseDown)
			{
				EInputComponent->BindAction(EIA_ReleaseDown, ETriggerEvent::Triggered, this, &UUreal2DClimbableComponent::ReleaseDown);
			}
		}
	}
}

void UUreal2DClimbableComponent::EnterClimbMode_Implementation()
{
	if (GetOwnerPlayerCharacter()->HasAuthority())
	{
		UCharacterMovementComponent* MovementComponent = GetOwnerPlayerCharacter()->GetCharacterMovement();

		MovementComponent->SetMovementMode(EMovementMode::MOVE_Walking);
	}
}

void UUreal2DClimbableComponent::EndClimbMode_Implementation()
{
	if (GetOwnerPlayerCharacter()->HasAuthority())
	{
		UCharacterMovementComponent* MovementComponent = GetOwnerPlayerCharacter()->GetCharacterMovement();
		if (GetOwnerPlayerCharacter()->GetCharacterMovement()->MovementMode == EMovementMode::MOVE_None)
		{
			MovementComponent->SetMovementMode(EMovementMode::MOVE_Walking);
		}
	}
}

void UUreal2DClimbableComponent::ClimbableHandle_Implementation(bool ToUp)
{
	if (GetOwnerPlayerCharacter()->HasAuthority())
	{
		if (!LeftRelease || !RightRelease)
		{
			// 如果在攀爬时，同时按下 "左/右" 则不处理攀爬输入
			return;
		}
		const FVector PlayerLocation = GetOwnerPlayerCharacter()->GetActorLocation();

		float ClimbValue = ClimbSpeed * ClimbRate;

		FVector ClimbLocation = CurrentClimbableActor->CalClimbPosition(PlayerLocation, ClimbValue, ToUp);

		if (CurrentClimbableActor->ClimbableType == EClimbableType::Wall)
		{
			ClimbingType = EClimbableType::Wall;

			UPaperFlipbookComponent* FlipbookComponent = GetOwnerPlayerCharacter()->GetSprite();
			if (ToUp)
			{
				FlipbookComponent->SetFlipbook(FB_EdgeGrab);
			} else
			{
				FlipbookComponent->SetFlipbook(FB_WallEdgeSlide);
			}
		}
		else if(CurrentClimbableActor->ClimbableType == EClimbableType::Vine)
		{
			ClimbingType = EClimbableType::Vine;
		}
		else if(CurrentClimbableActor->ClimbableType == EClimbableType::Ladder)
		{
			ClimbingType = EClimbableType::Ladder;
		}

		// 手动设置攀爬位置
		GetOwnerPlayerCharacter()->SetActorLocation(ClimbLocation);
		
		ClimbingState = EFlipBookClimbingState::Climbing;
		
		if (CurrentClimbableActor->IsToEnd(ClimbLocation.Z, ToUp))
		{
			// 如果到终点自动切换过渡状态，不再处于攀爬状态

			// 结束攀爬状态
			EndClimbState();
		}
	}
}

void UUreal2DClimbableComponent::FlipBookClimbingStateMachine()
{
	UPaperFlipbookComponent* FlipbookComponent = GetOwnerPlayerCharacter()->GetSprite();
	
	switch (ClimbingState)
	{
		case EFlipBookClimbingState::Climbing:
			{
				if (ClimbingType == EClimbableType::Ladder)
				{
					FlipbookComponent->SetFlipbook(FB_LadderClimb);
				}
				else if(ClimbingType == EClimbableType::Vine)
				{
					FlipbookComponent->SetFlipbook(FB_LadderClimb);
				}
				else if(ClimbingType == EClimbableType::Wall)
				{
					// 由于墙攀爬上下方向序列不一样，因此放置在用户输入时，根据攀爬方向设置
				}
				break;
			}
		case EFlipBookClimbingState::Idle:
			{
				if (ClimbingType == EClimbableType::Ladder)
				{
					FlipbookComponent->SetFlipbook(FB_LadderClimbPose);
				}
				else if(ClimbingType == EClimbableType::Vine)
				{
					FlipbookComponent->SetFlipbook(FB_LadderClimbPose);
				}
				else if(ClimbingType == EClimbableType::Wall)
				{
					FlipbookComponent->SetFlipbook(FB_EdgeIdle);
				}
				break;
			}
		default:
			{
					
			}
	}
}

AUnreal2DCharacter* UUreal2DClimbableComponent::GetOwnerPlayerCharacter()
{
	return Cast<AUnreal2DCharacter>(GetOwner());
}

void UUreal2DClimbableComponent::ClimbLeftMovement(const FInputActionValue& ActionValue)
{
	LeftRelease = false;
	// TODO 暂未设计攀爬时向左移动
}

void UUreal2DClimbableComponent::ClimbRightMovement(const FInputActionValue& ActionValue)
{
	RightRelease = false;
	// TODO 暂未设计攀爬时向右移动
}

void UUreal2DClimbableComponent::ClimbUpMovement(const FInputActionValue& ActionValue)
{
	UpRelease = false;
	if (bIsCanClimb && CurrentClimbableActor != nullptr)
	{
		ClimbableHandle(true);
	}
}

void UUreal2DClimbableComponent::ClimbDownMovement(const FInputActionValue& ActionValue)
{
	DownRelease = false;
	if (bIsCanClimb && CurrentClimbableActor != nullptr)
	{
		ClimbableHandle(false);
	}
}

void UUreal2DClimbableComponent::ClimbToJump()
{
	// 先判断组合按键多的跳跃

	FVector ImpulseForce;
	if (!LeftRelease && !UpRelease && RightRelease)
	{
		// 左上跳跃
		ImpulseForce.X = XImpulseForceValue * -1;
		ImpulseForce.Y = 0;
		ImpulseForce.Z = ZImpulseForceValue;
	}
	else if (!RightRelease && !UpRelease && LeftRelease)
	{
		// 右上跳跃
		ImpulseForce.X = XImpulseForceValue;
		ImpulseForce.Y = 0;
		ImpulseForce.Z = ZImpulseForceValue;
	}
	else if (!RightRelease && UpRelease && DownRelease)
	{
		// 往右跳跃
		ImpulseForce.X = XImpulseForceValue;
		ImpulseForce.Y = 0;
		ImpulseForce.Z = ZImpulseForceValue * SecondDirectionRatio;
	}
	else if (!LeftRelease && UpRelease  && DownRelease)
	{
		// 往左跳跃
		ImpulseForce.X = XImpulseForceValue * -1;
		ImpulseForce.Y = 0;
		ImpulseForce.Z = ZImpulseForceValue * SecondDirectionRatio;
	}
	else
	{
		// 往下落，同时结束攀爬
		ImpulseForce.X = 0;
		ImpulseForce.Y = 0;
		ImpulseForce.Z = 0;
	}

	// 处理按键，重置其状态
	LeftRelease = true;
	RightRelease = true;
	UpRelease = true;
	DownRelease = true;
	UPaperFlipbookComponent* FlipbookComponent = GetOwnerPlayerCharacter()->GetSprite();
	FlipbookComponent->SetFlipbook(GetOwnerPlayerCharacter()->FB_Falling);
	// 应用冲力
	ApplyClibToJump(ImpulseForce);
	
}

void UUreal2DClimbableComponent::ApplyClibToJump_Implementation(FVector ImpulseForce)
{
	EndClimbState();

	GetOwnerPlayerCharacter()->bWasJumping = true;
	GetOwnerPlayerCharacter()->GetCharacterMovement()->AddImpulse(ImpulseForce);
}
