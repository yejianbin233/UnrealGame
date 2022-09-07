// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemOperatorWidget.h"

#include "BackpackWidget.h"
#include "ItemOptionsWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/VerticalBoxSlot.h"
#include "UnrealGame/Backpack/ItemInfoObject.h"
#include "UnrealGame/Backpack/BackpackComponent.h"
#include "UnrealGame/Character/BlasterCharacter.h"

void UItemOperatorWidget::Reset(UItemInfoObject* NewItemInfoObject, TArray<FItemOptions>& Optionses)
{
	ItemOptionsBox->ClearChildren();

	ItemInfoObject = NewItemInfoObject;
	if (ItemInfoObject)
	{
		for (int I=0; I<Optionses.Num(); I++)
		{
			if (BackpackComponent->PlayerController)
			{
				UItemOptionsWidget* OptionsWidget = CreateItemOptionWidget();
				if (OptionsWidget)
				{
					OptionsWidget->Init(Optionses[I]);
					if (Optionses[I].OptionType == EItemOptionsType::Use)
					{
						// 使用
						OptionsWidget->OptionButton->OnClicked.AddDynamic(this, &ThisClass::Use);
					}

					if (Optionses[I].OptionType == EItemOptionsType::Equip)
					{
						// 装备
						OptionsWidget->OptionButton->OnClicked.AddDynamic(this, &ThisClass::Use);
					}

					if (Optionses[I].OptionType == EItemOptionsType::Show)
					{
						// 显示
						OptionsWidget->OptionButton->OnClicked.AddDynamic(this, &ThisClass::Show);
					}

					if (Optionses[I].OptionType == EItemOptionsType::Throw)
					{
						// 丢弃
						OptionsWidget->OptionButton->OnClicked.AddDynamic(this, &ThisClass::Throw);
					}

					if (Optionses[I].OptionType == EItemOptionsType::Cancle)
					{
						// 取消操作
						OptionsWidget->OptionButton->OnClicked.AddDynamic(this, &ThisClass::Cancel);
					}

					
					ItemOptionsBox->AddChildToVerticalBox(OptionsWidget);

					UVerticalBoxSlot* VerticalBoxSlot = UWidgetLayoutLibrary::SlotAsVerticalBoxSlot(OptionsWidget);
					VerticalBoxSlot->Size.SizeRule = ESlateSizeRule::Fill;
				}
			}
		}
	}
}

void UItemOperatorWidget::Use()
{
	if (ItemInfoObject && ItemInfoObject->UseItem)
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(GetOwningPlayer()))
		{
			if (ABlasterCharacter* PlayerCharacger = Cast<ABlasterCharacter>(PlayerController->GetPawn()))
			{
				ItemInfoObject->UseItem->Use(PlayerCharacger);
			};
		}
	}
}

void UItemOperatorWidget::Equip()
{
	if (ItemInfoObject && ItemInfoObject->UseItem)
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(GetOuter()))
		{
			if (ABlasterCharacter* PlayerCharacger = Cast<ABlasterCharacter>(PlayerController->GetPawn()))
			{
				ItemInfoObject->UseItem->Use(PlayerCharacger);
			};
		}
	}
}

void UItemOperatorWidget::Show()
{
	if (BackpackComponent && BackpackComponent->BackpackWidget && BackpackComponent->BackpackWidget->ItemRender)
	{
		BackpackComponent->BackpackWidget->ItemRender->SetRenderItemInfo(ItemInfoObject->SceneItem->GetDTItemInfo());

		// 当前显示则不显示，当前不显示则显示
		bIsShowItenRender = !bIsShowItenRender;
		BackpackComponent->BackpackWidget->ShowItemRender(bIsShowItenRender);
	}
}

void UItemOperatorWidget::UnShow()
{
	// 
	BackpackComponent->BackpackWidget->ShowItemRender(false);
}

void UItemOperatorWidget::Throw()
{
	BackpackComponent->CC_TryRemoveItem(ItemInfoObject->BackpackId);
	BackpackComponent->CC_CreateItemAfterDiscard(ItemInfoObject->SceneItem);

	// 取消显示操作控件
	Cancel();
}

void UItemOperatorWidget::Cancel()
{
	// this->RemoveFromViewport();
	BackpackComponent->BackpackWidget->CanvasPanel->RemoveChild(this);

	if (bIsShowItenRender)
	{
		// 当取消时，同时取消显示物品
		bIsShowItenRender = !bIsShowItenRender;
		BackpackComponent->BackpackWidget->ShowItemRender(bIsShowItenRender);
	}
}
