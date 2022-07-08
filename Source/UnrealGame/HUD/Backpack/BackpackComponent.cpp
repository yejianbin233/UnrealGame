// Fill out your copyright notice in the Description page of Project Settings.


#include "BackpackComponent.h"

#include "BackpackWidget.h"
#include "ItemBase.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
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
		Temp.SetDefaultData();

		// TODO Warning：初始化 Item 数组时，使用拥有默认数据的 Item 填充，如果不设置默认数据，默认为 nullptr 会存在 Bug，导致 UE 编辑器崩溃。
		Items[Index] = Temp;
	}
}


// Called every frame
void UBackpackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UBackpackComponent::TryAddItem(FBackpackItemInfo& InItem)
{
	int Index = 0;

	if (InItem.bIsCanStacking)
	{
		TArray<FPositionItem> InBackpackItems;
		GetItems(&InBackpackItems, InItem.Id);
		
		int TempItemNum = InItem.Num;
		
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

		if (InItem.Num != TempItemNum)
		{
			OnBackpackItemChanged.Broadcast();
		}
		
		InItem.Num = TempItemNum;
		// 如果叠加完所有已在背包的同名物品仍有剩余，则执行新加一格物品操作。
		
	} else
	{
		// 检测是否有不可重叠的重复武器，如果存在，则不可拾取
		TArray<FPositionItem> InBackpackItems;
		GetItems(&InBackpackItems, InItem.Id);

		if (InBackpackItems.Num() > 0)
		{
			return false;
		}
	}

	bool Result = this->IsHadPlace(InItem, &Index);

	if (Result)
	{
		// UE_LOG(LogTemp, Warning, TEXT("Add Index = %f"), Index)
		AddNewItem(&InItem, Index);

		OnBackpackItemChanged.Broadcast();
		
		return true;
	}

	return false;
}

bool UBackpackComponent::TryInsertItem(FBackpackItemInfo Item, int Index)
{
	if (PlaceIndexCheck(Item, Index))
	{
		AddNewItem(&Item, Index);

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

bool UBackpackComponent::IsHadPlace(FBackpackItemInfo Item, int* InIndex)
{
	for(int Index=0; Index < Items.Num(); Index++)
	{
		FBackpackItemInfo* IndexItem = &Items[Index];

		if (IndexItem->Id.IsEmpty())
		{
			bool Result = this->PlaceIndexCheck(Item, Index);
			if (Result)
			{
				*InIndex = Index;

				return true;
			}
		}
	}

	return false;
}

bool UBackpackComponent::PlaceIndexCheck(FBackpackItemInfo Item, int InIndex)
{
	if (!UKismetMathLibrary::InRange_IntInt(InIndex, 0, Items.Num(), true, false))
	{
		// 数组下标越界直接返回 false
		return false;
	}
	
	int X = Item.OccupyCellXYLength.X;
	int Y = Item.OccupyCellXYLength.Y;

	int TempIndex = InIndex;
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
		int PlaceIndex = InIndex + Column * I;
		
		for(int J=0; J < X; J++)
		{
			int Step = J > 0 ? 1:0;
			PlaceIndex += Step;

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

void UBackpackComponent::CoordinateConvert(int* InIndex, FVector2D* InPosition, bool* bToIndex)
{
	FVector2D Position(-1, -1);
	
	if (*bToIndex)
	{
		int Index = -1;

		int X = InPosition->X;
		int Y = InPosition->Y * Column;
		Index = X + Y;

		*InIndex = Index;
	}
	else
	{
		int X = *InIndex % Column;
		int Y = *InIndex / Column;

		
		Position.X = X;
		Position.Y = Y;

		InPosition->X = Position.X;
		InPosition->Y = Position.Y;
	}
}

void UBackpackComponent::PackpackCoordinateConvert(FVector2D BackpackWidgetLocalPosition, FVector2D& Position)
{
	int X = BackpackWidgetLocalPosition.X / CellSize;
	int Y = BackpackWidgetLocalPosition.Y / CellSize;

	Position.X = X;
	Position.Y = Y;
}

int UBackpackComponent::GetIndexByBackpackLocalPosition(FVector2D BackpackWidgetLocalPosition)
{

	int Index = -1;
	FVector2D Position(0, 0);
	bool bToIndex = true;
	
	PackpackCoordinateConvert(BackpackWidgetLocalPosition, Position);
	
	CoordinateConvert(&Index, &Position, &bToIndex);
	
	// 判断是否越界，如果越界则返回 -1 代表处理失败
	if (UKismetMathLibrary::InRange_IntInt(Index, 0, Items.Num(), true, false))
	{
		return Index;
	}
	else
	{
		Index = -1;
		return Index;
	}
}

FVector2D UBackpackComponent::GetBackpackLocalPositionByIndex(int Index)
{
	FVector2D Position;

	bool bToIndex = false;
	
	CoordinateConvert(&Index, &Position, &bToIndex);
	
	FVector2D LocalPosition(Position.X * CellSize, Position.Y * CellSize);

	return LocalPosition;
}

int UBackpackComponent::PlaceBackpackTipsBoxByIndex(int InIndex, FIntPoint OccupyCellXYLength)
{
	int Index = InIndex;
	FVector2D Position;
	bool bToIndex = false;
	
	CoordinateConvert(&Index, &Position, &bToIndex);

	int HalfX = OccupyCellXYLength.X / 2;
	int HalfY = OccupyCellXYLength.Y / 2;


	int ReduceX = 0;
	if (Position.X < HalfX)
	{
		ReduceX = Position.X;
	} else
	{
		ReduceX = HalfX;
	}

	int ReduceY = 0;
	if (Position.Y < HalfY)
	{
		ReduceY = Position.Y * Column;
	} else
	{
		ReduceY = HalfY * Column;
	}
	
	int ResultIndex = InIndex - ReduceX - ReduceY;


	// UE_LOG(LogTemp, Warning, TEXT("HalfX : %d, HalfY : %d"), HalfX, HalfY);
	// UE_LOG(LogTemp, Warning, TEXT("ReduceX : %d, ReduceY : %d"), ReduceX, ReduceY);
	// UE_LOG(LogTemp, Warning, TEXT("Result: %d"), ResultIndex);
	
	return ResultIndex;
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

			int TempIndex = Index;
			CoordinateConvert(&TempIndex, &PositionItem.Position, &bToIndex);

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

			int TempIndex= Index;
			CoordinateConvert(&TempIndex, &PositionItem.Position, &bToIndex);

			PositionItems->Add(PositionItem);
		}
	}
}

bool UBackpackComponent::IsValidPosition(FVector2D Position)
{
	bool ColumValid = UKismetMathLibrary::InRange_IntInt(0, Position.X, Column-1, true, true);
	bool RowValid = UKismetMathLibrary::InRange_IntInt(0, Position.Y, Row-1, true, true);

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

bool UBackpackComponent::RemoveItem(FString BackpackId)
{
	bool RemoveResult = false;
	if (BackpackId.IsEmpty())
	{
		return RemoveResult;
	}

	
	for (int Index=0; Index < Items.Num(); Index++)
	{
		FBackpackItemInfo Temp = Items[Index];

		if (Temp.BackpackId.Equals(BackpackId))
		{
			FBackpackItemInfo DefaultItem;
			DefaultItem.SetDefaultData();

			// 使用默认 Item 数据代替被移除的 Item。
			Items[Index] = DefaultItem;

			RemoveResult = true;
		}
	}

	return RemoveResult;
}

void UBackpackComponent::TryRemoveItem(FString BackpackId)
{
	bool RemoveResult = RemoveItem(BackpackId);

	if (RemoveResult)
	{
		OnBackpackItemChanged.Broadcast();
	}
}

void UBackpackComponent::CreateItemAfterDiscard(FString Id)
{
	if (BlasterCharacter)
	{
		FVector PlayerLocation = BlasterCharacter->GetActorLocation();

		FItemInfo ItemInfo = BlasterCharacter->GetItemInfoFromTable(FName(Id));

		if (ItemInfo.ItemBaseClass != nullptr)
		{
			if (ItemInfo.ItemBaseClass->GetClass() != nullptr)
			{
				AItemBase* Item = GetWorld()->SpawnActor<AItemBase>(ItemInfo.ItemBaseClass, PlayerLocation, FRotator(0.f));

				Item->InitMesh(ItemInfo);
			}
		}
		
	}
}
