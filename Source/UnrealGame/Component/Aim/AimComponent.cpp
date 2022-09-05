// Fill out your copyright notice in the Description page of Project Settings.


#include "AimComponent.h"

#include "Camera/CameraComponent.h"
#include "Components/TimelineComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "UnrealGame/Character/BlasterCharacter.h"

// Sets default values for this component's properties
UAimComponent::UAimComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	AimEndedInterpTimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("结束瞄准插值时间轴组件"));
	// ...
}


// Called when the game starts
void UAimComponent::BeginPlay()
{
	AimEndedInterpTimelineComponent->RegisterComponent();
	
	Super::BeginPlay();
	AWeapon* Weapon = Cast<AWeapon>(GetOwner());
	if (Weapon == nullptr)
	{
		return;
	}
	
	PlayerCharacter = Cast<ABlasterCharacter>(Weapon->GetPlayerCharacter());
	
	if (PlayerCharacter)
	{
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

void UAimComponent::AimStartedFOVInterp_Implementation(float InterpValue)
{
	if (!bIsAiming)
	{
		return;
	}
	
	InterpValue = FMath::Clamp(InterpValue, 0, 1);

	if (PlayerCharacter)
	{
		float CurrentFOV = PlayerCharacter->GetFollowCamera()->FieldOfView;
		float NextFOV = UKismetMathLibrary::Lerp(CurrentFOV, AimFOV, InterpValue);
		PlayerCharacter->GetFollowCamera()->SetFieldOfView(NextFOV);
	}
}

void UAimComponent::AimEndedFOVInterp_Implementation(float InterpValue)
{
	if (bIsAiming)
	{
		return;
	}
	
	InterpValue = FMath::Clamp(InterpValue, 0, 1);

	if (PlayerCharacter)
	{
		float CurrentFOV = PlayerCharacter->GetFollowCamera()->FieldOfView;
		float NextFOV = UKismetMathLibrary::Lerp(CurrentFOV, DefaultFOV, InterpValue);
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
	// WidgetAim(bToAim);
	CameraAim(bToAim);
}



