// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"

#include "BackpackComponent.h"
#include "ItemWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"


void UInventoryWidget::Init(UBackpackComponent* InBackpackComponent)
{
	BackpackComponent = InBackpackComponent;
	if (ensure(BackpackComponent))
	{
		CellSize = InBackpackComponent->CellSize;

		SetBackpackBorderSize();

		InitLinks();

		Refresh();

		BackpackComponent->OnBackpackItemChanged.AddUObject(this, &ThisClass::Refresh);
	}
}

void UInventoryWidget::SetBackpackBorderSize()
{
	UCanvasPanelSlot* CanvasPanelSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(BackpackBorder);

	FVector2D Size;
	Size.X = BackpackComponent->Column * BackpackComponent->CellSize;
	Size.Y = BackpackComponent->Row * BackpackComponent->CellSize;
	CanvasPanelSlot->SetSize(Size);
}

void UInventoryWidget::InitLinks()
{
	int Column = BackpackComponent->Column;
	int Row = BackpackComponent->Row;

	float RowLength = Column * CellSize;
	float ColumnLength = Row * CellSize;
	
	for (int RowIndex=0; RowIndex < Row; RowIndex++)
	{
		FVector2D Start;
		Start.X = 0;
		Start.Y = CellSize * RowIndex;
		FVector2D End;
		End.X = RowLength;
		End.Y = Start.Y;

		FLink Link;
		Link.Start = Start;
		Link.End = End;

		Links.Add(Link);
	}
	
	for (int ColumnIndex=0; ColumnIndex < Column; ColumnIndex++)
	{
		FVector2D Start;
		Start.X = CellSize * ColumnIndex;
		Start.Y = 0;
		FVector2D End;
		End.X = Start.X;
		End.Y = ColumnLength;
		
		FLink Link;
		Link.Start = Start;
		Link.End = End;

		Links.Add(Link);
	}

	
}

void UInventoryWidget::Refresh()
{
	if (BackpackComponent)
	{
		BackpackContent->ClearChildren();

		TArray<FPositionItem> PositionItems;
		
		BackpackComponent->GetAllItem(&PositionItems);

		
		for (int Index=0; Index < PositionItems.Num(); Index++)
		{
			FPositionItem IndexItem = PositionItems[Index];

			UItemWidget* ItemWidget =  Cast<UItemWidget>(CreateWidget(GetOwningPlayer(), InventoryUIClass));
			
			ItemWidget->Init(*IndexItem.Item, IndexItem.Position);

			UCanvasPanelSlot* ItemCanvasPanelSlot = BackpackContent->AddChildToCanvas(ItemWidget);

			FVector2D Position(ItemWidget->Position.X * CellSize, ItemWidget->Position.Y * CellSize);
			ItemCanvasPanelSlot->SetPosition(Position);

			FVector2D Size;
			Size.X = IndexItem.Item->OccupyCellXYLength.X * CellSize;
			Size.Y = IndexItem.Item->OccupyCellXYLength.Y * CellSize;
			ItemCanvasPanelSlot->SetSize(Size);
		}
	}
}
