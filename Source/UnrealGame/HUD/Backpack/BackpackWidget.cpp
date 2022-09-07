// Fill out your copyright notice in the Description page of Project Settings.

#include "BackpackWidget.h"

#include "ItemRenderWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SizeBox.h"
#include "UnrealGame/Backpack/BackpackComponent.h"

void UBackpackWidget::CreateItemRender_Implementation()
{
	if (BackpackComponent)
	{
		ItemRender = BackpackComponent->GetWorld()->SpawnActor<AItemRender>(ItemRenderClass);
		ItemRender->PlayerController = GetOwningPlayer();
		ItemRender->BindItemRenderInput();
	}
}

void UBackpackWidget::ShowItemRender_Implementation(bool bIsShow)
{
	if (bIsShow)
	{
		ItenRenderSizeBox->AddChild(Cast<UUserWidget>(ItemRenderWidget));
	}
	else
	{
		ItenRenderSizeBox->RemoveChild(Cast<UUserWidget>(ItemRenderWidget));
		ItemRenderWidget->DisableItemRenderInput();
	}
}
