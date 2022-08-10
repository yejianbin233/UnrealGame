// Fill out your copyright notice in the Description page of Project Settings.


#include "BackpackComponent.h"

#include "BackpackLagCompensationComponent.h"
#include "../HUD/Backpack/BackpackWidget.h"
#include "ItemBase.h"
#include "Blueprint/UserWidget.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

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
	PlayerCharacter = Cast<ABlasterCharacter>(GetOwner());

	PlayerController = Cast<APlayerController>(PlayerCharacter->GetController());

	int CellNum = Row * Column;
	Items.SetNum(CellNum);
	for (int Index=0; Index < CellNum; Index++)
	{
		FBackpackItemInfo Temp;
		Temp.SetDefaultData();

		// TODO Warning：初始化 Item 数组时，使用拥有默认数据的 Item 填充，如果不设置默认数据，默认为 nullptr 会存在 Bug，导致 UE 编辑器崩溃。
		Items[Index] = Temp;
	}

	if (!PlayerCharacter->HasAuthority())
	{
		PlayerCharacter->GetBackpackLagCompensationComponent()->OnServerReportBackpackDataOverride.AddUObject(this, &ThisClass::ServerReportBackpackDataOverride);
	}
}


// Called every frame
void UBackpackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UBackpackComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

EPickupResult UBackpackComponent::Pickup(AItemBase* PickItem)
{
	if (PickItem == nullptr)
	{
		// 如果拾取时，服务器的拾取物品为 nullptr，说明在服务器的对应 Item 已经被销毁(被拾取)，直接不进行处理，由 UE 自动更新该可复制 Actor 的信息。
		return EPickupResult::Fail;
	}

	FBackpackItemInfo BackpackItemInfo = PickItem->ActualItemInfo;
	int OriginNum = BackpackItemInfo.Num;

	bool PickupResult = TryAddItem(BackpackItemInfo);

	if (!PickupResult && BackpackItemInfo.CanRotate())
	{
		// 旋转物品
		BackpackItemInfo.Rotate();
		PickupResult = TryAddItem(BackpackItemInfo);
	}
	// 全部拾取
	if (PickupResult)
	{
		return EPickupResult::All;
	}
	else
	{
		// 拾取失败
		if (OriginNum != BackpackItemInfo.Num)
		{
			// 部分拾取
			return EPickupResult::Part;
		}
		else
		{
			return EPickupResult::Fail;
		}
	}
}

// void UBackpackComponent::CC_Pickup_Implementation(AItemBase* PickItem)
// {
// 	if (PickItem == nullptr)
// 	{
// 		// 如果拾取时，服务器的拾取物品为 nullptr，说明在服务器的对应 Item 已经被销毁(被拾取)，直接不进行处理，由 UE 自动更新该可复制 Actor 的信息。
// 		return EPickupResult::Fail;
// 	}
//
// 	FBackpackItemInfo BackpackItemInfo = PickItem->ActualItemInfo;
// 	int OriginNum = BackpackItemInfo.Num;
//
// 	bool PickupResult = TryAddItem(BackpackItemInfo);
//
// 	if (!PickupResult && BackpackItemInfo.CanRotate())
// 	{
// 		// 旋转物品
// 		BackpackItemInfo.Rotate();
// 		PickupResult = TryAddItem(BackpackItemInfo);
// 	}
// 	// 全部拾取
// 	if (PickupResult)
// 	{
// 		return EPickupResult::All;
// 	}
// 	else
// 	{
// 		// 拾取失败
// 		if (OriginNum != BackpackItemInfo.Num)
// 		{
// 			// 部分拾取
// 			return EPickupResult::Part;
// 		}
// 		else
// 		{
// 			return EPickupResult::Fail;
// 		}
// 	}
// }


void UBackpackComponent::SC_Pickup_Implementation(AItemBase* PickItem, float BackpackItemChangedTime)
{
	EPickupResult PickupResult = Pickup(PickItem);

	if (PickupResult == EPickupResult::All)
	{
		// // 当服务器成功拾取物品时，广播
		OnBackpackItemChanged.Broadcast();
		// 服务器销毁
		PickItem->Destroy();
	}
	else if (PickupResult == EPickupResult::Part)
	{
		// // 当服务器成功部分拾取物品时，广播
		OnBackpackItemChanged.Broadcast();

		// 是否为了确保万无一失，调用 ServerFeedbackPickupItemFailture ?
		UBackpackLagCompensationComponent* BackpackLagCompensationComponent = PlayerCharacter->GetBackpackLagCompensationComponent();
		if (BackpackLagCompensationComponent)
		{
			BackpackLagCompensationComponent->ServerFeedbackPickupItemFailture(PickItem);
		}
	}
	else
	{
		/*
		 * 拾取失败的情况：
		 *
		 * 1. 背包没有添加任何物品数据
		 * 2. 背包添加了部分物品，对于该种情况需要在服务器修改物品的数量(客户端修改，同时在服务器也会修改，但服务器会覆盖客户端)
		 */ 
		// 部分添加仅修改数据，恢复客户端的可视性
		UBackpackLagCompensationComponent* BackpackLagCompensationComponent = PlayerCharacter->GetBackpackLagCompensationComponent();
		if (BackpackLagCompensationComponent)
		{
			BackpackLagCompensationComponent->ServerFeedbackPickupItemFailture(PickItem);
		}
	}

	if (PlayerCharacter)
	{
		UBackpackLagCompensationComponent* BackpackLagCompensationComponent = PlayerCharacter->GetBackpackLagCompensationComponent();

		if (BackpackLagCompensationComponent)
		{
			BackpackLagCompensationComponent->ServerFeedbackBackpackItemChangedResult(BackpackItemChangedTime);
		}
	}
}

void UBackpackComponent::SNC_Pickup(AItemBase* PickItem)
{
	EPickupResult PickupResult = Pickup(PickItem);
	if (PickupResult == EPickupResult::All)
	{
		PickItem->Destroy();
	}
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
		// 如果叠加完所有已在背包的同名物品仍有剩余，则执行新加一格物品操作
	}
	else
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
		AddNewItem(InItem, Index);

		OnBackpackItemChanged.Broadcast();
		
		return true;
	}

	return false;
}

void UBackpackComponent::TryInsertItem(FBackpackItemInfo Item, int Index)
{
	if (PlaceIndexCheck(Item, Index))
	{
		AddNewItem(Item, Index);

		OnBackpackItemChanged.Broadcast();
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


void UBackpackComponent::GetAllItem(TArray<FPositionItem>& PositionItems)
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
			PositionItems.Add(PositionItem);
		}
	}
}

void UBackpackComponent::GetItems(TArray<FPositionItem>* PositionItems, FString Id)
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

void UBackpackComponent::CC_UpdateItemNum_Implementation(const FString& BackpackId, int Num)
{
	if (Num == 0)
	{
		// 当数量为 0 时，删除
		CC_TryRemoveItem(BackpackId);
	}
	
	for (int I=0; I<Items.Num(); I++)
	{
		if (Items[I].BackpackId == BackpackId)
		{
			Items[I].Num = Num;
			break;
		}
	}
	// 刷新 UI
	OnBackpackItemChanged.Broadcast();
	
	// 记录客户端更新
	float ClientUpdateTime = GetWorld()->GetTimeSeconds();
	OnClientBackpackItemChanged.Broadcast(ClientUpdateTime);
}

void UBackpackComponent::SC_UpdateItemNum_Implementation(const FString& BackpackId, int Num, float ClientUpdateTime)
{
	if (Num == 0)
	{
		// 当数量为 0 时，删除
		SC_TryRemoveItem(BackpackId, ClientUpdateTime);
	}
	
	// 服务器更新数量
	for (int I=0; I<Items.Num(); I++)
	{
		if (Items[I].BackpackId == BackpackId)
		{
			Items[I].Num = Num;
			break;
		}
	}
	// 向客户端反馈更新结果
	OnServerReportBackpackItemChanged.Broadcast(ClientUpdateTime);
}

void UBackpackComponent::SNC_UpdateItemNum(const FString& BackpackId, int Num)
{
	if (Num == 0)
	{
		// 当数量为 0 时，删除
		SNC_TryRemoveItem(BackpackId);
	}
	// 服务器作为客户端，直接修改数量，且更新 UI
	for (int I=0; I<Items.Num(); I++)
	{
		if (Items[I].BackpackId == BackpackId)
		{
			Items[I].Num = Num;
			break;
		}
	}
	OnBackpackItemChanged.Broadcast();
}

bool UBackpackComponent::IsValidPosition(FVector2D Position)
{
	bool ColumValid = UKismetMathLibrary::InRange_IntInt(0, Position.X, Column-1, true, true);
	bool RowValid = UKismetMathLibrary::InRange_IntInt(0, Position.Y, Row-1, true, true);

	return ColumValid && RowValid;
}

void UBackpackComponent::OpenOrCloseBackpack_Implementation()
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

void UBackpackComponent::SNC_TryRemoveItem(const FString& BackpackId)
{
	bool RemoveResult = RemoveItem(BackpackId);
	if (RemoveResult)
	{
		OnBackpackItemChanged.Broadcast();
	}
}

void UBackpackComponent::CC_TryRemoveItem_Implementation(const FString& BackpackId)
{
	if (PlayerCharacter->HasAuthority())
	{
		SNC_TryRemoveItem(BackpackId);
	}
	else
	{
		bool RemoveResult = RemoveItem(BackpackId);
		if (RemoveResult)
		{
			OnBackpackItemChanged.Broadcast();
		}
		float RemoveTime = GetWorld()->GetTimeSeconds();
		OnClientBackpackItemChanged.Broadcast(RemoveTime);
		SC_TryRemoveItem(BackpackId, RemoveTime);
	}
	
}


void UBackpackComponent::SC_TryRemoveItem_Implementation(const FString& BackpackId, float RemoveTime)
{
	bool RemoveResult = RemoveItem(BackpackId);

	if (RemoveResult)
	{
		OnBackpackItemChanged.Broadcast();
		OnServerReportBackpackItemChanged.Broadcast(RemoveTime);
	}
}

void UBackpackComponent::CC_CreateItemAfterDiscard_Implementation(const FString& Id)
{
	
	if (PlayerCharacter)
	{
		if (PlayerCharacter->HasAuthority())
		{
			SNC_CreateItemAfterDiscard(Id);
		}
		else
		{
			FVector PlayerLocation = PlayerCharacter->GetActorLocation();

			FItemInfo ItemInfo = PlayerCharacter->GetItemInfoFromTable(FName(Id));
		
			if (ItemInfo.ItemClass != nullptr)
			{
				if (ItemInfo.ItemClass->GetClass() != nullptr)
				{
					FActorSpawnParameters ActorSpawnParameters;
					ActorSpawnParameters.Name = FName(*FString::Printf(TEXT("Item-%f"), GetWorld()->GetTimeSeconds()));

					FRotator ActorRotator(0,0,0);
					AItemBase* Item = Cast<AItemBase>(GetWorld()->SpawnActor(ItemInfo.ItemClass, &PlayerLocation, &ActorRotator, ActorSpawnParameters));
				
					Item->Init(ItemInfo);
					
					// 非服务器创建的物品都是不可拾取的，不可复制，等到服务器创建对应的物品将其替换
					Item->PickableAreaComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
					Item->SetReplicates(false);
					SC_CreateItemAfterDiscard(Id, FName(*Item->GetName()));
				}
			}
		}
	}
}

void UBackpackComponent::SC_CreateItemAfterDiscard_Implementation(const FString& Id, FName ItemClientName)
{
	SNC_CreateItemAfterDiscard(Id);
	
	UBackpackLagCompensationComponent* BackpackLagCompensationComponent = PlayerCharacter->GetBackpackLagCompensationComponent();
	if (BackpackLagCompensationComponent)
	{
		BackpackLagCompensationComponent->ServerReportClientCreateItemResult(ItemClientName);
	}
}

void UBackpackComponent::SNC_CreateItemAfterDiscard(const FString& Id)
{
	if (PlayerCharacter)
	{
		FVector PlayerLocation = PlayerCharacter->GetActorLocation();

		FItemInfo ItemInfo = PlayerCharacter->GetItemInfoFromTable(FName(Id));
		
		if (ItemInfo.ItemClass != nullptr)
		{
			if (ItemInfo.ItemClass->GetClass() != nullptr)
			{
				FActorSpawnParameters ActorSpawnParameters;
				ActorSpawnParameters.Name = FName(*FString::Printf(TEXT("Item-%f"), GetWorld()->GetTimeSeconds()));

				FRotator ActorRotator(0,0,0);
				AItemBase* Item = Cast<AItemBase>(GetWorld()->SpawnActor(ItemInfo.ItemClass, &PlayerLocation, &ActorRotator, ActorSpawnParameters));
				
				Item->Init(ItemInfo);
			}
		}
	}
}

void UBackpackComponent::GetItemsByType(EItemType InItemType, TArray<FBackpackItemInfo>& InItems)
{
	for (auto Item : Items)
	{
		if (Item.ItemType == InItemType)
		{
			InItems.Add(Item);
		}
	}
}

void UBackpackComponent::ServerReportBackpackDataOverride(const TArray<FBackpackItemInfo>& ServerBackpackItems)
{
	Items.Empty();
	Items.Append(ServerBackpackItems);
	OnBackpackItemChanged.Broadcast();
}

void UBackpackComponent::AddNewItem(FBackpackItemInfo Item, int Index)
{
	int X = Item.OccupyCellXYLength.X;
	int Y = Item.OccupyCellXYLength.Y;
	Item.BackpackId = FString::Printf(TEXT("%d"), Index);
	
	for(int I=0; I < Y; I++)
	{
		int PlaceIndex = Index + Column * I;
		
		for(int J=0; J < X; J++)
		{
			int TempJ = FMath::Min(1, J);
			PlaceIndex += TempJ;
			
			Items[PlaceIndex] = Item;
		}
	}
}
