// Fill out your copyright notice in the Description page of Project Settings.


#include "CrosshairComponent.h"

#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "UnrealGame/Character/BlasterCharacter.h"
#include "UnrealGame/HUD/Collimation/CrosshairWidget.h"

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

void UCrosshairComponent::UpdateSpread(float InSpread)
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

void UCrosshairComponent::Init(ABlasterCharacter* InPlayerCharacter)
{
	Super::Init(InPlayerCharacter);

	if (ensureMsgf(PlayerCharacter != nullptr, TEXT("准星组件未设置玩家角色引用")))
	{
		APlayerController* LocalZeroPlayerController = UGameplayStatics::GetPlayerControllerFromID(GetWorld(),0);

		APlayerController* PlayerController = Cast<APlayerController>(PlayerCharacter->GetController());

		if (LocalZeroPlayerController == PlayerController)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, TEXT("equal"));
		} else
		{
			GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, TEXT("not equal"));
		}

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
}

