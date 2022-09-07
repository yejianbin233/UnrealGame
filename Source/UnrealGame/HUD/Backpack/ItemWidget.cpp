// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemWidget.h"

#include "BackpackWidget.h"
#include "ItemOperatorWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "UnrealGame/Backpack/BackpackComponent.h"
#include "UnrealGame/Enums/UnrealGameEnumInterface.h"

void UItemWidget::ShowItemOperate(UItemInfoObject* ItemInfoObject, FVector2D Position)
{
	if (ItemInfoObject)
	{
		// TODO - 默认物品有以下三种使用方式
		TArray<FItemOptions> Optionses;
		FItemOptions UseOption;
		UseOption.OptionType = EItemOptionsType::Use;
		UseOption.OptionName = TEXT("使用");
		Optionses.Add(UseOption);

		FItemOptions ShowOption;
		ShowOption.OptionType = EItemOptionsType::Show;
		ShowOption.OptionName = TEXT("展示");
		Optionses.Add(ShowOption);

		FItemOptions ThrowOption;
		ThrowOption.OptionType = EItemOptionsType::Throw;
		ThrowOption.OptionName = TEXT("丢弃");
		Optionses.Add(ThrowOption);

		FItemOptions CancelOption;
		CancelOption.OptionType = EItemOptionsType::Cancle;
		CancelOption.OptionName = TEXT("取消");
		Optionses.Add(CancelOption);

		if (BackpackComponent && BackpackComponent->BackpackWidget && BackpackComponent->BackpackWidget->ItemOperatorWidget)
		{
			BackpackComponent->BackpackWidget->ItemOperatorWidget->Reset(ItemInfoObject, Optionses);

			UCanvasPanelSlot* CanvasPanelSlot = BackpackComponent->BackpackWidget->CanvasPanel->AddChildToCanvas(BackpackComponent->BackpackWidget->ItemOperatorWidget);

			CanvasPanelSlot->SetPosition(Position);

			CanvasPanelSlot->SetSize(Size);
		};
	}
}
