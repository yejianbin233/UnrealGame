// Fill out your copyright notice in the Description page of Project Settings.


#include "CrosshairComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "UnrealGame/Character/BlasterCharacter.h"
#include "UnrealGame/HUD/Collimation/CrosshairWidget.h"

void UCrosshairComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UCrosshairComponent::Init(ABlasterCharacter* InPlayerCharacter)
{
	Super::Init(InPlayerCharacter);

	if (ensureMsgf(PlayerCharacter != nullptr, TEXT("准星组件未设置玩家角色引用")))
	{
		APlayerController* LocalZeroPlayerController = UGameplayStatics::GetPlayerControllerFromID(GetWorld(),0);

		if (LocalZeroPlayerController)
		{
			CrosshairWidget = CreateWidget<UCrosshairWidget>(LocalZeroPlayerController, CrosshairWidgetClass, FName(TEXT("Collimation")));

			if (CrosshairWidget)
			{
				CrosshairWidget->CrosshairsCenter->SetBrushFromTexture(CrosshairsCenter, true);
				CrosshairWidget->CrosshairsTop->SetBrushFromTexture(CrosshairsTop, true);
				CrosshairWidget->CrosshairsRight->SetBrushFromTexture(CrosshairsRight, true);
				CrosshairWidget->CrosshairsBottom->SetBrushFromTexture(CrosshairsBottom, true);
				CrosshairWidget->CrosshairsLeft->SetBrushFromTexture(CrosshairsLeft, true);
			} else
			{
				GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, TEXT("Create CrosshairWidget Fail!"));
			}
		}
	}

	if (PlayerCharacter->HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(RecoverTimerHandle, this, &UCrosshairComponent::FireOverheatRecoverHandle, RecoverInterval, true);
		GetWorld()->GetTimerManager().SetTimer(FireCountReduceTimerHandle, this, &UCrosshairComponent::FireCountReduceHandle, FireCountReduceInterval, true);
	}
}

void UCrosshairComponent::ShowCollimation()
{
	if (CrosshairWidget)
	{
		CrosshairWidget->AddToViewport();
	}
}

void UCrosshairComponent::HideCollimation()
{
	if (CrosshairWidget)
	{
		CrosshairWidget->RemoveFromViewport();
	}
}

void UCrosshairComponent::AddFireCount()
{
	bIsAllowRecover = false;
	if (GetWorld()->GetTimerManager().TimerExists(AllowRecoverTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(AllowRecoverTimerHandle);
	}
	GetWorld()->GetTimerManager().SetTimer(AllowRecoverTimerHandle, this, &ThisClass::AllowRecoverHandle, DelayAllowRecoverTime, false, DelayAllowRecoverTime);
	
	FireCount = FMath::Clamp(FireCount + 1, FireCountMin, FireCountMax);

	float FireOverHeatValue = FireOverHeatCurve->GetFloatValue(FireCount);

	UpdateSpread(FireOverHeatValue);
}


void UCrosshairComponent::FireCountReduceHandle()
{
	if (FireCount == 0)
	{
		return;
	}

	if (!bIsAllowRecover)
	{
		return;
	}
	
	const float FireCountDecayValue = FireCountDecayCurve->GetFloatValue(FireCount);
	
	FireCount = FMath::Clamp(FireCount-FireCountDecayValue, FireCountMin, FireCountMax);
}

void UCrosshairComponent::FireOverheatRecoverHandle()
{
	if (!bIsAllowRecover)
	{
		return;
	}
	
	float OverHeatRecoverValue = OverHeatRecoverCurve->GetFloatValue(FireCount);\
	float NewSpreadValue = CurrentSpreadValue - OverHeatRecoverValue;
	UpdateSpread(NewSpreadValue);
}

void UCrosshairComponent::AllowRecoverHandle()
{
	bIsAllowRecover = true;
}

void UCrosshairComponent::UpdateSpread(float SpreadCurveValue)
{
	// FVector2D TopTransform = CrosshairWidget->CrosshairsTop->RenderTransform.Translation;
	// FVector2D RightTransform = CrosshairWidget->CrosshairsRight->RenderTransform.Translation;
	// FVector2D BottonTransform = CrosshairWidget->CrosshairsBottom->RenderTransform.Translation;
	// FVector2D LeftTransform = CrosshairWidget->CrosshairsLeft->RenderTransform.Translation;
	//
	// float TopSpreadCurveValue = FMath::Abs(FMath::Max(0, FMath::Abs(TopTransform.Y) + SpreadCurveValue));
	// float RightSpreadCurveValue = FMath::Abs(FMath::Max(0, FMath::Abs(RightTransform.X) + SpreadCurveValue));
	// float BottonSpreadCurveValue = FMath::Abs(FMath::Max(0, FMath::Abs(BottonTransform.Y) + SpreadCurveValue));
	// float LeftSpreadCurveValue = FMath::Abs(FMath::Max(0, FMath::Abs(LeftTransform.X) + SpreadCurveValue));
	//
	// UE_LOG(LogTemp, Warning, TEXT("SpreadCurveValue: %f"), SpreadCurveValue);
	// CrosshairWidget->CrosshairsTop->SetRenderTranslation(FVector2D(0, -TopSpreadCurveValue));
	// CrosshairWidget->CrosshairsRight->SetRenderTranslation(FVector2D(RightSpreadCurveValue, 0));
	// CrosshairWidget->CrosshairsBottom->SetRenderTranslation(FVector2D(0, BottonSpreadCurveValue));
	// CrosshairWidget->CrosshairsLeft->SetRenderTranslation(FVector2D(-LeftSpreadCurveValue, 0));
	CurrentSpreadValue = FMath::Max(0, SpreadCurveValue);
	CrosshairWidget->CrosshairsTop->SetRenderTranslation(FVector2D(0, -SpreadCurveValue));
	CrosshairWidget->CrosshairsRight->SetRenderTranslation(FVector2D(SpreadCurveValue, 0));
	CrosshairWidget->CrosshairsBottom->SetRenderTranslation(FVector2D(0, SpreadCurveValue));
	CrosshairWidget->CrosshairsLeft->SetRenderTranslation(FVector2D(-SpreadCurveValue, 0));
}