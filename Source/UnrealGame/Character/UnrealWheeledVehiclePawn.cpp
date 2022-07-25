// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealWheeledVehiclePawn.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "ChaosVehicleMovementComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/SpringArmComponent.h"

AUnrealWheeledVehiclePawn::AUnrealWheeledVehiclePawn()
{
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	VehicleAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("VehicleAudioComponent"));

	SpringArmComponent->SetupAttachment(RootComponent);
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	VehicleAudioComponent->SetupAttachment(RootComponent);
}

void AUnrealWheeledVehiclePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			EnhancedInputSubSystem->ClearAllMappings();

			EnhancedInputSubSystem->AddMappingContext(VehicleInputMappingContext, 0);


			// Binding

			if (UEnhancedInputComponent* EInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
			{
				if (EIA_Vehicle_MoveForward)
				{
					EInputComponent->BindAction(EIA_Vehicle_MoveForward, ETriggerEvent::Triggered, this, &ThisClass::MoveForward);
				}

				if (EIA_Vehicle_Right)
				{
					EInputComponent->BindAction(EIA_Vehicle_Right, ETriggerEvent::Triggered, this, &ThisClass::MoveRight);
				}

				if (EIA_Vehicle_Brake)
				{
					EInputComponent->BindAction(EIA_Vehicle_Brake, ETriggerEvent::Triggered, this, &ThisClass::Brake);
				}

				if (EIA_Vehicle_HandBrake)
				{
					EInputComponent->BindAction(EIA_Vehicle_HandBrake, ETriggerEvent::Triggered, this, &ThisClass::PressedHandBrake);
				}

				if (EIA_Vehicle_CancleHandBrake)
				{
					EInputComponent->BindAction(EIA_Vehicle_CancleHandBrake, ETriggerEvent::Triggered, this, &ThisClass::ReleaseHandBrake);
				}
			}
		}
	}
}

void AUnrealWheeledVehiclePawn::MoveForward(const FInputActionValue& ActionValue)
{

	GetVehicleMovementComponent()->SetThrottleInput(ActionValue.GetMagnitude());
}

void AUnrealWheeledVehiclePawn::MoveRight(const FInputActionValue& ActionValue)
{
	GetVehicleMovementComponent()->SetSteeringInput(ActionValue.GetMagnitude());
}

void AUnrealWheeledVehiclePawn::Brake(const FInputActionValue& ActionValue)
{
	GetVehicleMovementComponent()->SetBrakeInput(ActionValue.GetMagnitude());
}

void AUnrealWheeledVehiclePawn::PressedHandBrake()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(true);
}

void AUnrealWheeledVehiclePawn::ReleaseHandBrake()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(false);
}

