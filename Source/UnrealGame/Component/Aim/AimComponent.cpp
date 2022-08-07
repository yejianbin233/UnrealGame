// Fill out your copyright notice in the Description page of Project Settings.


#include "AimComponent.h"

#include "Camera/CameraComponent.h"
#include "Components/TimelineComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "UnrealGame/Character/BlasterCharacter.h"
#include "UnrealGame/PlayerController/BlasterPlayerController.h"

// Sets default values for this component's properties
UAimComponent::UAimComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// AimStartedInterpTimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("开始瞄准插值时间轴组件"));
	AimEndedInterpTimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("结束瞄准插值时间轴组件"));
	AimEndedInterpTimelineComponent->RegisterComponent();
	
	
	// ...
}


// Called when the game starts
void UAimComponent::BeginPlay()
{
	Super::BeginPlay();
	PlayerCharacter = Cast<ABlasterCharacter>(GetOwner());
	if (PlayerCharacter)
	{
		PlayerController = Cast<ABlasterPlayerController>(PlayerCharacter->GetController());
		DefaultFOV = PlayerCharacter->GetFollowCamera()->FieldOfView;
	}
	// ...
	if (AimStartedInterpCurve)
	{
		OnAimStarted.BindDynamic(this, &ThisClass::UAimComponent::AimStartedFOVInterp);
		AimStartedInterpTimelineComponent.AddInterpFloat(AimStartedInterpCurve, OnAimStarted);
	}

	if (AimEndedInterpCurve)
	{
		OnAimEnded.BindDynamic(this, &ThisClass::UAimComponent::AimEndedFOVInterp);
		AimEndedInterpTimelineComponent->AddInterpFloat(AimEndedInterpCurve, OnAimEnded);
	}
}

void UAimComponent::AimStartedFOVInterp(float InterpValue)
{
	if (!bIsAiming)
	{
		return;
	}
	
	InterpValue = FMath::Clamp(InterpValue, 0, 1);

	float CurrentFOV = PlayerCharacter->GetFollowCamera()->FieldOfView;
	float NextFOV = UKismetMathLibrary::Lerp(CurrentFOV, AimFOV, InterpValue);

	if (PlayerCharacter)
	{
		PlayerCharacter->GetFollowCamera()->SetFieldOfView(NextFOV);
	}
}

void UAimComponent::AimEndedFOVInterp(float InterpValue)
{
	if (bIsAiming)
	{
		return;
	}
	
	InterpValue = FMath::Clamp(InterpValue, 0, 1);

	float CurrentFOV = PlayerCharacter->GetFollowCamera()->FieldOfView;
	float NextFOV = UKismetMathLibrary::Lerp(CurrentFOV, DefaultFOV, InterpValue);
	
	if (PlayerCharacter)
	{
		PlayerCharacter->GetFollowCamera()->SetFieldOfView(NextFOV);
	}
}

void UAimComponent::CameraAim(bool bToAim)
{
	if (bToAim)
	{
		AimStartedInterpTimelineComponent.PlayFromStart();
	} else
	{
		AimEndedInterpTimelineComponent->PlayFromStart();
	}
}

// Called every frame
void UAimComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	AimStartedInterpTimelineComponent.TickTimeline(DeltaTime);
	AimEndedInterpTimelineComponent->TickComponent(DeltaTime, ELevelTick::LEVELTICK_TimeOnly, NULL);
	// ...
}

void UAimComponent::Aiming(bool bToAim)
{
	bIsAiming = bToAim;
	WidgetAim(bToAim);
	CameraAim(bToAim);
}



