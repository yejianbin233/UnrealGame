// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractivePointLight.h"

#include "Components/PointLightComponent.h"

AInteractivePointLight::AInteractivePointLight()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PointLightComponent = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLightComponent"));

	PointLightComponent->SetupAttachment(RootComponent);
}

void AInteractivePointLight::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	PointLightComponent->bUseIESBrightness = false;

	if (FlashLightFunctionMaterialInstanceDynamic)
	{
		PointLightComponent->SetLightFunctionMaterial(FlashLightFunctionMaterialInstanceDynamic);
	}
}

// Called when the game starts or when spawned
void AInteractivePointLight::BeginPlay()
{
	Super::BeginPlay();
	
}

void AInteractivePointLight::TurnOn()
{
	PointLightComponent->Intensity = DefaultLightIntensity;

	// 如果使用了灯光 IES 纹理，且使用 IES 强度而不是使用灯光强度
	if (PointLightComponent->IESTexture.Get())
	{
		// 只要
		// PointLightComponent->bUseIESBrightness = bLightTurnOnUseIESBrightness;
	}

	// 当重新打开手电筒时，恢复灯光周期闪烁功能
	if (bIsFlickerLooping)
	{
		PeroidicFlickerTimeline.PlayFromStart();
	}
}

void AInteractivePointLight::TurnOff()
{
	// 灯光强度为 0
	PointLightComponent->Intensity = 0.0f;

	if (PeroidicFlickerTimeline.IsPlaying())
	{
		PeroidicFlickerTimeline.Stop();
	}
}

void AInteractivePointLight::Flicker(ELightFlickerType NewLightFlickerType)
{
	OnLightFlickerStared.Broadcast(NewLightFlickerType);
}

void AInteractivePointLight::EndFlicker()
{
	OnLightFlickerEnded.Broadcast();
}

void AInteractivePointLight::ChangeLightColor()
{
	
}
