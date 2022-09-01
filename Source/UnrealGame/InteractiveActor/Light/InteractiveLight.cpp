// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractiveLight.h"

// Sets default values
AInteractiveLight::AInteractiveLight()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	SetRootComponent(MeshComponent);
}

// Called when the game starts or when spawned
void AInteractiveLight::BeginPlay()
{
	Super::BeginPlay();
}

void AInteractiveLight::Interactive()
{
	IInteractiveInterface::Interactive();

	if (InteractiveState == EInteractiveState::Interactable)
	{
		// 可交互状态
		const bool bIsTurnOn = LightState == ELightState::TurnOn;
		if (bIsTurnOn)
		{
			TurnOff();
		}
		else
		{
			TurnOn();
		}
	}
}

void AInteractiveLight::PeriodicFlickerHandle(float Value)
{
	if (LightState == ELightState::TurnOn)
	{
		FlashLightFunctionMaterialInstanceDynamic->SetScalarParameterValue(FlickerValueName, Value);
		// TODO 测试是否需要重新设置材质
		// FlashLightComponent->SetLightFunctionMaterial(FlashLightFunctionMaterialInstanceDynamic);
	}
}

void AInteractiveLight::TemporaryFlickerHandle(float Value)
{
	if (LightState == ELightState::TurnOn)
	{
		FlashLightFunctionMaterialInstanceDynamic->SetScalarParameterValue(FlickerValueName, Value);
		// TODO 测试是否需要重新设置材质
		// FlashLightComponent->SetLightFunctionMaterial(FlashLightFunctionMaterialInstanceDynamic);
	}
}

void AInteractiveLight::StartLightFlicker(ELightFlickerType InLightFlickerType)
{
	LightFlickerType = InLightFlickerType;

	// 只要灯光希望开始闪烁，那么将忽略灯光当前闪烁，重新执行灯光闪烁
	if (LightFlickerType == ELightFlickerType::Temporary)
	{
		bIsFlickerLooping = false;

		if (TemporaryFlickerTimeline.IsPlaying())
		{
			TemporaryFlickerTimeline.Stop();
		}

		if (PeroidicFlickerTimeline.IsPlaying())
		{
			PeroidicFlickerTimeline.Stop();
		}
		// 临时灯光闪烁
		TemporaryFlickerTimeline.PlayFromStart();
	}
	else if(LightFlickerType == ELightFlickerType::Periodic)
	{
		bIsFlickerLooping = true;

		if (TemporaryFlickerTimeline.IsPlaying())
		{
			TemporaryFlickerTimeline.Stop();
		}

		if (PeroidicFlickerTimeline.IsPlaying())
		{
			PeroidicFlickerTimeline.Stop();
		}

		// 周期灯光闪烁
		PeroidicFlickerTimeline.PlayFromStart();
	}
}

void AInteractiveLight::EndLightFlicker()
{
	if (TemporaryFlickerTimeline.IsPlaying())
	{
		TemporaryFlickerTimeline.Stop();
	}

	if (PeroidicFlickerTimeline.IsPlaying())
	{
		PeroidicFlickerTimeline.Stop();
	}
}

// Called every frame
void AInteractiveLight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TemporaryFlickerTimeline.TickTimeline(DeltaTime);
	PeroidicFlickerTimeline.TickTimeline(DeltaTime);
}

void AInteractiveLight::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	if (TemporaryFlickerCurve)
	{
		OnTemporaryFlickerStarted.BindDynamic(this, &ThisClass::TemporaryFlickerHandle);
		TemporaryFlickerTimeline.AddInterpFloat(TemporaryFlickerCurve, OnTemporaryFlickerStarted);
	}

	if (PeriodicFlickerCurve)
	{
		OnPeroidicFlickerStarted.BindDynamic(this, &ThisClass::PeriodicFlickerHandle);

		// 循环
		PeroidicFlickerTimeline.SetLooping(true);
		PeroidicFlickerTimeline.AddInterpFloat(PeriodicFlickerCurve, OnPeroidicFlickerStarted);
	}

	OnLightFlickerStared.AddUObject(this, &ThisClass::StartLightFlicker);

	if (FlashLightFunctionMaterialInstance)
	{
		FlashLightFunctionMaterialInstanceDynamic = UMaterialInstanceDynamic::Create(FlashLightFunctionMaterialInstance, this);
	}
}

