// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractiveSpotLight.h"
#include "Components/SpotLightComponent.h"
#include "UnrealGame/Component/Light/LightAbilityComponent.h"

// Sets default values
AInteractiveSpotLight::AInteractiveSpotLight()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpotLightComponent = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLightComponent"));

	SpotLightComponent->SetupAttachment(RootComponent);
}

void AInteractiveSpotLight::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	SpotLightComponent->bUseIESBrightness = false;

	if (FlashLightFunctionMaterialInstanceDynamic)
	{
		SpotLightComponent->SetLightFunctionMaterial(FlashLightFunctionMaterialInstanceDynamic);
	}
}

// Called when the game starts or when spawned
void AInteractiveSpotLight::BeginPlay()
{
	Super::BeginPlay();

}

void AInteractiveSpotLight::TurnOn()
{
	SpotLightComponent->Intensity = DefaultLightIntensity;

	// 当重新打开手电筒时，恢复灯光周期闪烁功能
	if (bIsFlickerLooping)
	{
		PeroidicFlickerTimeline.PlayFromStart();
	}
}

void AInteractiveSpotLight::TurnOff()
{
	// 灯光强度为 0
	SpotLightComponent->Intensity = 0.0f;

	if (PeroidicFlickerTimeline.IsPlaying())
	{
		PeroidicFlickerTimeline.Stop();
	}
}

void AInteractiveSpotLight::Flicker(ELightFlickerType NewLightFlickerType)
{
	OnLightFlickerStared.Broadcast(NewLightFlickerType);
}

void AInteractiveSpotLight::EndFlicker()
{
	OnLightFlickerEnded.Broadcast();
}

void AInteractiveSpotLight::ChangeLightColor()
{
	
}


