// Fill out your copyright notice in the Description page of Project Settings.


#include "CrosshairComponent.h"

#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "UnrealGame/Character/BlasterCharacter.h"
#include "UnrealGame/HUD/Collimation/CrosshairWidget.h"

UCrosshairComponent::UCrosshairComponent()
{

	BlasterCharacter = Cast<ABlasterCharacter>(GetOwner());

	if (ensure(BlasterCharacter != nullptr))
	{
		APlayerController* PlayerController = Cast<APlayerController>(BlasterCharacter->GetController());

		
		if (ensure(PlayerController != nullptr))
		{
			CrosshairWidget = CreateWidget<UCrosshairWidget>(PlayerController, CrosshairWidgetClass, FName(TEXT("Collimation")));

			CrosshairWidget->CrosshairsCenter->SetBrushFromTexture(CrosshairsCenter, true);
			CrosshairWidget->CrosshairsTop->SetBrushFromTexture(CrosshairsTop, true);
			CrosshairWidget->CrosshairsRight->SetBrushFromTexture(CrosshairsRight, true);
			CrosshairWidget->CrosshairsBottom->SetBrushFromTexture(CrosshairsBottom, true);
			CrosshairWidget->CrosshairsLeft->SetBrushFromTexture(CrosshairsLeft, true);
		}
	}
}

void UCrosshairComponent::ShowCollimation()
{
	CrosshairWidget->AddToViewport();
}

void UCrosshairComponent::HideCollimation()
{
	CrosshairWidget->RemoveFromViewport();
}

void UCrosshairComponent::UpdateSpread()
{
	float CurveTimeMin;
	float CurveTimeMax;
	CrosshairOffsetCurve->GetTimeRange(CurveTimeMin, CurveTimeMax);

	Spread = FMath::Clamp(Spread, CurveTimeMin, CurveTimeMax);
	
	float SpreadCurveValue = CrosshairOffsetCurve->GetFloatValue(Spread);
	
	CrosshairWidget->CrosshairsTop->SetRenderTranslation(FVector2D(0, -SpreadCurveValue));
	CrosshairWidget->CrosshairsRight->SetRenderTranslation(FVector2D(SpreadCurveValue, 0));
	CrosshairWidget->CrosshairsBottom->SetRenderTranslation(FVector2D(0, SpreadCurveValue));
	CrosshairWidget->CrosshairsLeft->SetRenderTranslation(FVector2D(-SpreadCurveValue, 0));
}

void UCrosshairComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
	UpdateSpread();
}
