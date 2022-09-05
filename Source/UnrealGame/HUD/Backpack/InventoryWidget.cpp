// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"

#include "../../Backpack/BackpackComponent.h"
#include "ItemWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/KismetMathLibrary.h"


void UInventoryWidget::Init(UBackpackComponent* InBackpackComponent)
{
	BackpackComponent = InBackpackComponent;
	if (ensure(BackpackComponent))
	{
		CellSize = InBackpackComponent->CellSize;

		SetBackpackBorderSize();

		InitLinks();

		Refresh();

		BackpackComponent->BackpackInventoryWidget = this;
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
	
	for (int RowIndex=0; RowIndex <= Row; RowIndex++)
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
	
	for (int ColumnIndex=0; ColumnIndex <= Column; ColumnIndex++)
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
		
		BackpackComponent->GetAllItem(PositionItems);

		
		for (int Index=0; Index < PositionItems.Num(); Index++)
		{
			FPositionItem IndexItem = PositionItems[Index];

			UItemWidget* ItemWidget =  CreateItemWidget(IndexItem.Position, IndexItem.Item);
			
			UCanvasPanelSlot* ItemCanvasPanelSlot = BackpackContent->AddChildToCanvas(ItemWidget);

			FVector2D Position(IndexItem.Position.X * CellSize, IndexItem.Position.Y * CellSize);

			ItemCanvasPanelSlot->SetPosition(Position);
			ItemCanvasPanelSlot->SetAutoSize(true);
		}
	}
}

bool UInventoryWidget::IsValidBackpackArea(FVector2D LeftTopPoint, FVector2D RightBottomPoint, float EdgeSize)
{
	int Column = BackpackComponent->Column;
	int Row = BackpackComponent->Row;

	float BackpackColumnLength = Column * CellSize + EdgeSize;
	float BackpackRowLength = Row * CellSize  + EdgeSize;

	float Min = 0 - EdgeSize;
	
	bool LeftTopX = UKismetMathLibrary::InRange_FloatFloat(LeftTopPoint.X, Min, BackpackColumnLength, true, true);
	bool LeftTopY = UKismetMathLibrary::InRange_FloatFloat(LeftTopPoint.Y, Min, BackpackRowLength, true, true);

	bool RightBottomX = UKismetMathLibrary::InRange_FloatFloat(RightBottomPoint.X, Min, BackpackColumnLength, true, true);
	bool RightBottomY = UKismetMathLibrary::InRange_FloatFloat(RightBottomPoint.Y, Min, BackpackRowLength, true, true);

	// UE_LOG(LogTemp, Warning, TEXT("Min: %f"), Min);
	// UE_LOG(LogTemp, Warning, TEXT("BackpackColumnLength: %f    --  BackpackRowLength: %f"), BackpackColumnLength, BackpackRowLength);
	// UE_LOG(LogTemp, Warning, TEXT("LeftTopX: %f    --  LeftTopY: %f"), LeftTopPoint.X, LeftTopPoint.Y);
	// UE_LOG(LogTemp, Warning, TEXT("RightBottomX: %f    --  RightBottomY: %f"), RightBottomPoint.X, RightBottomPoint.Y);
	
	return LeftTopX && LeftTopY && RightBottomX && RightBottomY;
}

