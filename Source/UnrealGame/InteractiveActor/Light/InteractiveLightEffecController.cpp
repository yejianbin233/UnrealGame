// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractiveLightEffecController.h"

// Sets default values
AInteractiveLightEffecController::AInteractiveLightEffecController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
}

void AInteractiveLightEffecController::ApplyLightEffect(ELightEffectType NewLightEffecType)
{
	for (AInteractiveLight* Light : Lights)
	{
		// 暂时默认统一应用周期性灯光闪烁效果
		if (NewLightEffecType == ELightEffectType::Flicker)
		{
			Light->OnLightFlickerStared.Broadcast(LightFlickerType);
		}

		if (NewLightEffecType == ELightEffectType::Normal)
		{
			Light->OnLightFlickerEnded.Broadcast();
		}
	}
}


#if WITH_EDITOR
void AInteractiveLightEffecController::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	FName PropertyName = PropertyChangedEvent.Property != nullptr ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(AInteractiveLightEffecController, LightEffectType))
	{
		ApplyLightEffect(LightEffectType);
	}
}
#endif

// Called when the game starts or when spawned
void AInteractiveLightEffecController::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInteractiveLightEffecController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

