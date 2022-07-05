// Fill out your copyright notice in the Description page of Project Settings.


#include "BackpackComponent.h"

#include "BackpackWidget.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UBackpackComponent::UBackpackComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	Items.SetNum(0);
	// ...
}

// Called when the game starts
void UBackpackComponent::BeginPlay()
{
	Super::BeginPlay();
	// ...
	BlasterCharacter = Cast<ABlasterCharacter>(GetOwner());

	PlayerController = Cast<APlayerController>(BlasterCharacter->GetController());

	int CellNum = Row * Column;
	Items.SetNum(CellNum);
	for (int Index=0; Index < CellNum; Index++)
	{
		FBackpackItemInfo Temp;
		Temp.CreateDefaultData();
		Items[Index] = Temp;
	}
}


// Called every frame
void UBackpackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UBackpackComponent::TryAddItem(FBackpackItemInfo* InItem)
{
	int Index = 0;

	if (InItem->bIsCanStacking)
	{
		TArray<FPositionItem> InBackpackItems;
		GetItems(&InBackpackItems, InItem->Id);
		
		int TempItemNum = InItem->Num;
		
		for (int I=0; I<InBackpackItems.Num(); I++)
		{
			FPositionItem Temp = InBackpackItems[I];

			int CanAddNum = Temp.Item->MaxStackingNum - Temp.Item->Num;

			int ActualAddNum = FMath::Min(TempItemNum, CanAddNum);
			TempItemNum -= ActualAddNum;
			Temp.Item->Num = Temp.Item->Num + ActualAddNum;

			if (TempItemNum == 0)
			{
				return true;
			}
		}

		if (InItem->Num != TempItemNum)
		{
			OnBackpackItemChanged.Broadcast();
		}
		
		InItem->Num = TempItemNum;
		// 如果叠加完所有已在背包的同名物品仍有剩余，则执行新加一格物品操作。
		
	} else
	{
		// 检测是否有不可重叠的重复武器，如果存在，则不可拾取
		TArray<FPositionItem> InBackpackItems;
		GetItems(&InBackpackItems, InItem->Id);

		if (InBackpackItems.Num() > 0)
		{
			return false;
		}
	}

	bool Result = this->IsHadPlace(InItem, &Index);

	if (Result)
	{
		AddNewItem(InItem, Index);

		OnBackpackItemChanged.Broadcast();
		
		return true;
	}

	return false;
}

void UBackpackComponent::AddNewItem(FBackpackItemInfo* Item, int Index)
{
	int X = Item->OccupyCellXYLength.X;
	int Y = Item->OccupyCellXYLength.Y;
	Item->BackpackId = FString::Printf(TEXT("%d"), Index);
	
	for(int I=0; I < Y; I++)
	{
		int PlaceIndex = Index + Column * I;
		
		for(int J=0; J < X; J++)
		{
			int TempJ = FMath::Min(1, J);
			PlaceIndex += TempJ;
			
			Items[PlaceIndex] = *Item;
		}
	}
}

bool UBackpackComponent::IsHadPlace(FBackpackItemInfo* Item, int* InIndex)
{
	for(int Index=0; Index < Items.Num(); Index++)
	{
		FBackpackItemInfo* IndexItem = &Items[Index];

		if (IndexItem->Id.IsEmpty())
		{
			bool Result = this->PlaceCheck(Item, &Index);
			if (Result)
			{
				*InIndex = Index;
				return true;
			}
		}
	}

	return false;
}

bool UBackpackComponent::PlaceCheck(FBackpackItemInfo* Item, int* InIndex)
{
	int X = Item->OccupyCellXYLength.X;
	int Y = Item->OccupyCellXYLength.Y;

	int TempIndex = *InIndex;
	FVector2D Position;
	bool ToIndex = false;

	CoordinateConvert(&TempIndex, &Position, &ToIndex);

	float ActualColumn = Position.X + X;
	if (ActualColumn > Column)
	{
		return false;
	}

	float ActualRow = Position.Y + Y;
	if (ActualRow > Row)
	{
		return false;
	}

	for(int I=0; I < Y; I++)
	{
		int PlaceIndex = *InIndex + Column * I;
		
		for(int J=0; J < X; J++)
		{
			PlaceIndex += J;

			if (PlaceIndex >= Items.Num())
			{
				return false;
			}
			
			FBackpackItemInfo* PackageItem = &Items[PlaceIndex];
			
			if (!PackageItem->Id.IsEmpty())
			{
				return false;
			}
		}
	}
	return true;
}

void UBackpackComponent::CoordinateConvert(int* Index, FVector2D* Position, bool* bToIndex)
{
	if (bToIndex != nullptr)
	{
		bool TempToIndex = *bToIndex;

		if (TempToIndex)
		{
			*Index = Position->X + Position->Y * Column;
		} else
		{
			Position->X = *Index % Column;
			Position->Y = *Index / Column;
		}
	}
	else
	{
		Position->X = *Index % Column;
		Position->Y = *Index / Column;
	}
}

void UBackpackComponent::GetAllItem(TArray<FPositionItem>* PositionItems)
{
	TSet<FBackpackItemInfo, FStruct_SetKeyFuncs> ItemSet;

	for(int Index=0; Index < Items.Num(); Index++)
	{
		FBackpackItemInfo* IndexItem = &Items[Index];

		if (!IndexItem->Id.IsEmpty() && !ItemSet.Contains(IndexItem->BackpackId))
		{
			FPositionItem PositionItem;
			PositionItem.Item = IndexItem;

			bool bToIndex = false;
			CoordinateConvert(&Index, &PositionItem.Position, &bToIndex);

			ItemSet.Add(*IndexItem);
			PositionItems->Add(PositionItem);
		}
	}
}

inline void UBackpackComponent::GetItems(TArray<FPositionItem>* PositionItems, FString Id)
{
	for(int Index=0; Index < Items.Num(); Index++)
	{
		FBackpackItemInfo* IndexItem = &Items[Index];

		if (!IndexItem->Id.IsEmpty() && IndexItem->Id.Equals(Id))
		{
			FPositionItem PositionItem;
			PositionItem.Item = IndexItem;

			bool bToIndex = false;
			CoordinateConvert(&Index, &PositionItem.Position, &bToIndex);

			PositionItems->Add(PositionItem);
		}
	}
}

bool UBackpackComponent::IsValidPosition(FVector2D Position)
{
	bool ColumValid = UKismetMathLibrary::InRange_IntInt(0, Position.X, Column-1, true, true);
	bool RowValid = UKismetMathLibrary::InRange_IntInt(0, Position.X, Row-1, true, true);

	return ColumValid && RowValid;
}

void UBackpackComponent::OpenOrCloseBackpack()
{
	if (BackpackWidget == nullptr)
	{
		BackpackWidget = Cast<UBackpackWidget>(CreateWidget(PlayerController, BackpackWidgetClass));
	}

	if (BackpackWidget->IsVisible())
	{
		BackpackWidget->RemoveFromParent();

		PlayerController->bShowMouseCursor = false;
		FInputModeGameOnly GameOnly;
		
		PlayerController->SetInputMode(GameOnly);
		
	} else
	{
		FInputModeGameAndUI GameAndUI;
		if (BackpackWidget != nullptr)
		{
			GameAndUI.SetWidgetToFocus(BackpackWidget->TakeWidget());
		}
		GameAndUI.SetHideCursorDuringCapture(true);
		GameAndUI.SetLockMouseToViewportBehavior(EMouseLockMode::LockOnCapture);
		
		PlayerController->bShowMouseCursor = true;
		PlayerController->SetInputMode(GameAndUI);
		
		BackpackWidget->AddToViewport();
	}
}