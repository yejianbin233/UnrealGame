// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemRenderWidget.h"

#include "EnhancedInputSubsystems.h"
#include "UnrealGame/Character/BlasterCharacter.h"

void UItemRenderWidget::EnableItemRenderInput()
{
	if (APlayerController* PlayerController = GetOwningPlayer())
	{
		ItemRender->EnableInput(GetOwningPlayer());

		ItemRender->ResetRotator();
		ItemRender->ResetArmLength();
		
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();

			Subsystem->AddMappingContext(ItemRender->ItemRenderInputMappingContext, 0);
		}
	};
}

void UItemRenderWidget::DisableItemRenderInput()
{
	if (APlayerController* PlayerController = GetOwningPlayer())
	{
		ItemRender->DisableInput(GetOwningPlayer());

		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();

			// TODO 输入上下文切换可以定义一个子系统来管理。
			if (ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(PlayerController->GetPawn()))
			{
				Subsystem->AddMappingContext(BlasterCharacter->ShootGameInputMappingContext, 0);
			}
		}
	};
}
