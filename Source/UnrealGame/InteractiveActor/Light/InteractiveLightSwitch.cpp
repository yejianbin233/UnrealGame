// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractiveLightSwitch.h"

#include "UnrealGame/Interfaces/UnrealInterface.h"

// Sets default values
AInteractiveLightSwitch::AInteractiveLightSwitch()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
}

void AInteractiveLightSwitch::Switch()
{
	ELightState PreLightState = LightState;
	
	if (PreLightState == ELightState::TurnOff)
	{
		LightState = ELightState::TurnOn;
		TurnOn();
	}
	else if (PreLightState == ELightState::TurnOn)
	{
		LightState = ELightState::TurnOff;
		TurnOff();
	}
}

// Called when the game starts or when spawned
void AInteractiveLightSwitch::BeginPlay()
{
	Super::BeginPlay();
}

#if WITH_EDITOR
void AInteractiveLightSwitch::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	FName PropertyName = PropertyChangedEvent.Property != nullptr ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AInteractiveLightSwitch, LightState))
	{
		if (LightState == ELightState::TurnOn)
		{
			TurnOn();
		}
		else if (LightState == ELightState::TurnOff)
		{
			TurnOff();
		}
	}
}
#endif

void AInteractiveLightSwitch::TurnOn()
{
	// 统一打开
	for (AInteractiveLight* Light: Lights)
	{
		if (Light)
		{
			Light->TurnOn();
		}
	}
}

void AInteractiveLightSwitch::TurnOff()
{
	// 统一关闭
	for (AInteractiveLight* Light: Lights)
	{
		if (Light)
		{
			Light->TurnOff();
		}
	}
}

