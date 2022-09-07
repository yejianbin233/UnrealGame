// Fill out your copyright notice in the Description page of Project Settings.


#include "BackpackComponent.h"

#include "BackpackLagCompensationComponent.h"
#include "../HUD/Backpack/BackpackWidget.h"
#include "ItemBase.h"
#include "Blueprint/UserWidget.h"
#include "Engine/ActorChannel.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "UnrealGame/Component/Combat/CombatComponent.h"
#include "UnrealGame/HUD/Backpack/InventoryWidget.h"
#include "UnrealGame/HUD/Backpack/ItemOperatorWidget.h"

DEFINE_LOG_CATEGORY(BackpackLog)

// Sets default values for this component's properties
UBackpackComponent::UBackpackComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	Items.SetNum(0);
	// ...
}

bool UBackpackComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{

	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	// Реплицируем наш объект.
	// if (MyObject) WroteSomething |= Channel->ReplicateSubobject(MyObject , *Bunch, *RepFlags);

	return WroteSomething;
}

// Called when the game starts
void UBackpackComponent::BeginPlay()
{
	Super::BeginPlay();
	// ...
	PlayerCharacter = Cast<ABlasterCharacter>(GetOwner());

	PlayerController = Cast<APlayerController>(PlayerCharacter->GetController());

	const int CellNum = Row * Column;
	Items.SetNum(CellNum);
	
	if (!PlayerCharacter->HasAuthority())
	{
		PlayerCharacter->GetBackpackLagCompensationComponent()->OnServerReportBackpackDataOverride.AddUObject(this, &ThisClass::ServerReportBackpackDataOverride);
	}

	if (BackpackWidget == nullptr)
	{
		// 初始化设置
		if (BackpackWidgetClass && PlayerController)
		{
			UUserWidget* UserWidget = CreateWidget(PlayerController, BackpackWidgetClass);
			BackpackWidget = Cast<UBackpackWidget>(UserWidget);
			if (BackpackWidget)
			{
				BackpackWidget->BackpackComponent = this;

				// 创建物品渲染Actor
				BackpackWidget->CreateItemRender();

				// 创建物品渲染控件
				BackpackWidget->ItemRenderWidget = CreateWidget<UItemRenderWidget>(PlayerController, BackpackWidget->ItemRenderWidgetClass);
				if (BackpackWidget->ItemRenderWidget)
				{
					BackpackWidget->ItemRenderWidget->ItemRender = BackpackWidget->ItemRender;
				}
				
				BackpackWidget->ItemOperatorWidget = CreateWidget<UItemOperatorWidget>(PlayerController, BackpackWidget->ItemOperatorWidgetClass);
				if (BackpackWidget->ItemOperatorWidget)
				{
					BackpackWidget->ItemOperatorWidget->BackpackComponent = this;
				}
				
				// 初始化背包库存控件 
				BackpackWidget->InventoryWidget->Init(this);
			}
		}
	}
}

void UBackpackComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

FPickupResult UBackpackComponent::Pickup(AItemBase* PickItem)
{
	if (PickItem == nullptr)
	{
		// 如果拾取时，服务器的拾取物品为 nullptr，说明在服务器的对应 Item 已经被销毁(被拾取)，直接不进行处理，由 UE 自动更新该可复制 Actor 的信息。
		return FPickupResult(EPickupResult::Fail, nullptr,false);
	}

	FBackpackItemInfo BackpackItemInfo = PickItem->GetItemInfo();
	
	FPickupResult PickupResult = TryAddItem(PickItem);

	if (PickupResult.bIsNoSpace && BackpackItemInfo.CanRotate())
	{
		// 旋转物品
		BackpackItemInfo.Rotate();
		PickupResult = TryAddItem(PickItem);
	}
	
	return PickupResult;
}

// 客户端不允许直接拾取物品，统一在服务器进行实际拾取操作
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
	if (PlayerCharacter == nullptr)
	{
		return;
	}
	
	const FPickupResult PickupResult = Pickup(PickItem);

	ABlasterCharacter::DisplayRole(PlayerCharacter->GetLocalRole());

	UBackpackLagCompensationComponent* BackpackLagCompensationComponent = PlayerCharacter->GetBackpackLagCompensationComponent();

	if (PickupResult.Result == EPickupResult::AddNewItem_All
		|| PickupResult.Result == EPickupResult::StackAdd_All)
	{
		// // 当服务器成功拾取物品时，广播
		OnBackpackItemChanged.Broadcast();
		// 执行将物品放置到背包处理
		PickItem->PutInBackpackHandle();
	}
	else if (PickupResult.Result == EPickupResult::AddNewItem_Part
		|| PickupResult.Result == EPickupResult::StackAdd_Part)
	{
		// // 当服务器成功部分拾取物品时，广播
		OnBackpackItemChanged.Broadcast();

		// 调用 ServerFeedbackPickupItemFailture 反馈部分添加成功，部分失败，处理部分失败部分(部分添加仅修改数据，恢复客户端的可视性)
		if (BackpackLagCompensationComponent)
		{
			BackpackLagCompensationComponent->ServerFeedbackPickupItemFailture(PickItem);
		}
	}

	if(PickupResult.Result == EPickupResult::Fail
		|| PickupResult.Result == EPickupResult::BackpackUnique_Fail)
	{
		/*
		 * 拾取失败的情况：
		 *
		 * 1. 背包没有添加任何物品数据
		 * 2. 背包添加了部分物品，对于该种情况需要在服务器修改物品的数量(客户端修改，同时在服务器也会修改，但服务器会覆盖客户端)
		 */ 
		// 部分添加仅修改数据，恢复客户端的可视性
		if (BackpackLagCompensationComponent)
		{
			BackpackLagCompensationComponent->ServerFeedbackPickupItemFailture(PickItem);
		}
	}
	

	// 每次拾取服务器都主动更新客户端背包数据
	if (BackpackLagCompensationComponent)
	{
		BackpackLagCompensationComponent->ServerFeedbackBackpackItemChangedResult(BackpackItemChangedTime);
	}
}

void UBackpackComponent::SNC_Pickup(AItemBase* PickItem)
{
	FPickupResult PickupResult = Pickup(PickItem);
	
	if (PickupResult.Result == EPickupResult::StackAdd_All
		|| PickupResult.Result == EPickupResult::AddNewItem_All)
	{
		// 全部添加
		PickItem->PutInBackpackHandle();
	}
}

FPickupResult UBackpackComponent::TryAddItem(AItemBase* InSceneItem)
{
	int Index = 0;

	FBackpackItemInfo InItem = InSceneItem->GetItemInfo();
	
	bool bIsStackingAdd = false;
	if (InItem.bIsCanStacking)
	{
		int OriginItemNum = InItem.Num;
		int SurplusNum = InItem.Num;
		TArray<UItemInfoObject*> InBackpackItems;
		GetItems(InBackpackItems, InItem.Id);
		
		
		for (int I=0; I<InBackpackItems.Num(); I++)
		{
			FBackpackItemInfo* InBackpackItem = &InBackpackItems[I]->BackpackItemInfo;

			int CanAddNum = InBackpackItem->MaxStackingNum - InBackpackItem->Num;

			int ActualAddNum = FMath::Min(OriginItemNum, CanAddNum);
			SurplusNum -= ActualAddNum;
			InBackpackItem->Num = InBackpackItem->Num + ActualAddNum;

			if (SurplusNum == 0)
			{
				return FPickupResult(EPickupResult::StackAdd_All, nullptr, false);
			}
		}

		if (InItem.Num != SurplusNum)
		{
			OnBackpackItemChanged.Broadcast();

			bIsStackingAdd = true;
			InItem.Num = SurplusNum;
		}
		// 如果叠加完所有已在背包的同名物品仍有剩余，则执行新加一格物品操作
	}

	// 背包中的物品应该默认都是可重复放置，可根据配置处理
	if (InItem.bIsBackpackUnique)
	{
		// 检测是否有不可重叠的重复武器，如果存在，则不可拾取
		TArray<FPositionItem> InBackpackItems;
		GetItems(InBackpackItems, InItem.Id);

		
		if (InBackpackItems.Num() > 0)
		{
			if (bIsStackingAdd)
			{
				// 可叠加，但不可重复放置时，直接返回结果
				return FPickupResult(EPickupResult::StackAdd_Part, nullptr, false);
			}
			// 添加失败，存在相同物品
			return FPickupResult(EPickupResult::BackpackUnique_Fail, nullptr, false);
		}
	}

	const bool Result = this->IsHadPlace(InItem, &Index);
	if (Result)
	{
		AddNewItem(InSceneItem->GetItemInfoObject(), Index);

		OnBackpackItemChanged.Broadcast();
		
		return FPickupResult(EPickupResult::AddNewItem_All, InSceneItem->GetItemInfoObject(), false);
	}
	else
	{
		if (bIsStackingAdd)
		{
			// 如果已添加部分可堆叠物品
			return FPickupResult(EPickupResult::StackAdd_Part, nullptr, true);
		}
		else
		{
			// 添加失败，什么物品都没有添加到背包
			return FPickupResult(EPickupResult::Fail, nullptr, true);
		}
	}
}

bool UBackpackComponent::TryInsertItem(UItemInfoObject* Item, int Index)
{
	if (PlaceIndexCheck(Item->BackpackItemInfo, Index))
	{
		AddNewItem(Item, Index);

		OnBackpackItemChanged.Broadcast();
		return true;
	}
	return false;
}

bool UBackpackComponent::IsHadPlace(FBackpackItemInfo Item, int* InIndex)
{
	for(int Index=0; Index < Items.Num(); Index++)
	{
		const UItemInfoObject* IndexItem = Items[Index];
		
		if (IndexItem == nullptr)
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
	
	const int X = Item.OccupyCellXYLength.X;
	const int Y = Item.OccupyCellXYLength.Y;

	int TempIndex = InIndex;
	FVector2D Position;
	bool ToIndex = false;

	CoordinateConvert(&TempIndex, &Position, &ToIndex);
	
	const float ActualColumn = Position.X + X;
	if (ActualColumn > Column)
	{
		return false;
	}

	const float ActualRow = Position.Y + Y;
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
			
			UItemInfoObject* PackageItem = Items[PlaceIndex];
			// 只要放置范围内有一个单元格位置被占用，直接结束并返回 false
			if (PackageItem)
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

	return ResultIndex;
}


void UBackpackComponent::GetAllItem(TArray<FPositionItem>& PositionItems)
{
	TSet<UItemInfoObject*, FStruct_SetKeyFuncs> ItemSet;

	for(int Index=0; Index < Items.Num(); Index++)
	{
		UItemInfoObject* IndexItem = Items[Index];

		if (IndexItem == nullptr)
		{
			continue;
		}
		if (!IndexItem->BackpackItemInfo.Id.IsEmpty() && !ItemSet.Contains(IndexItem->BackpackId))
		{
			FPositionItem PositionItem;
			PositionItem.Item = IndexItem;

			bool bToIndex = false;

			int TempIndex = Index;
			CoordinateConvert(&TempIndex, &PositionItem.Position, &bToIndex);

			ItemSet.Add(IndexItem);
			PositionItems.Add(PositionItem);
		}
	}
}

void UBackpackComponent::GetItems(TArray<FPositionItem>& PositionItems, FString Id)
{
	for(int Index=0; Index < Items.Num(); Index++)
	{
		UItemInfoObject* IndexItem = Items[Index];

		if (IndexItem == nullptr)
		{
			return;
		}
		
		if (!IndexItem->BackpackItemInfo.Id.IsEmpty() && IndexItem->BackpackItemInfo.Id.Equals(Id))
		{
			FPositionItem PositionItem;
			PositionItem.Item = IndexItem;

			bool bToIndex = false;

			int TempIndex= Index;
			CoordinateConvert(&TempIndex, &PositionItem.Position, &bToIndex);

			PositionItems.Add(PositionItem);
		}
	}
}

void UBackpackComponent::GetItems(TArray<UItemInfoObject*>& ItemInfos, FString Id)
{
	TSet<FString> BackpackSet;
	
	for(int Index=0; Index < Items.Num(); Index++)
	{
		UItemInfoObject* ItemInfoObject = Items[Index];
		if (ItemInfoObject == nullptr)
		{
			return;
		}
		if (!BackpackSet.Contains(ItemInfoObject->BackpackId)
			&& !ItemInfoObject->BackpackItemInfo.Id.IsEmpty()
			&& ItemInfoObject->BackpackItemInfo.Id.Equals(Id))
		{
			BackpackSet.Add(ItemInfoObject->BackpackId);
			ItemInfos.Add(ItemInfoObject);
		}
	}
}



void UBackpackComponent::GetItemsByBackpackId(TArray<FBackpackItemInfo>& BackpackItemInfos, FString BackpackId)
{
	for(int Index=0; Index < Items.Num(); Index++)
	{
		UItemInfoObject* IndexItem = Items[Index];

		if (IndexItem == nullptr)
		{
			continue;
		}
		
		if (!IndexItem->BackpackId.IsEmpty() && IndexItem->BackpackId.Equals(BackpackId))
		{
			BackpackItemInfos.Add(IndexItem->BackpackItemInfo);
		}
	}
}

void UBackpackComponent::CC_UpdateItem_Implementation(const FString& BackpackId, FBackpackItemInfo NewItemInfo)
{
	if (NewItemInfo.Num == 0)
	{
		// 当数量为 0 时，删除
		CC_TryRemoveItem(BackpackId);
	}
	
	for (int I=0; I<Items.Num(); I++)
	{
		if (Items[I] && Items[I]->BackpackId == BackpackId)
		{
			Items[I]->BackpackItemInfo.Num = NewItemInfo.Num;
			Items[I]->BackpackItemInfo.LongRangeWeaponEquipAmmoNum = NewItemInfo.LongRangeWeaponEquipAmmoNum;
			break;
		}
	}
	// 刷新 UI
	OnBackpackItemChanged.Broadcast();
	
	// 记录客户端更新
	float ClientUpdateTime = GetWorld()->GetTimeSeconds();
	OnClientBackpackItemChanged.Broadcast(ClientUpdateTime);
}

void UBackpackComponent::SC_UpdateItem_Implementation(const FString& BackpackId, FBackpackItemInfo NewItemInfo, float ClientUpdateTime)
{
	if (NewItemInfo.Num == 0)
	{
		// 当数量为 0 时，删除
		SC_TryRemoveItem(BackpackId, ClientUpdateTime);
	}
	
	// 服务器更新数量
	for (int I=0; I<Items.Num(); I++)
	{
		if (Items[I] && Items[I]->BackpackId == BackpackId)
		{
			Items[I]->BackpackItemInfo.Num = NewItemInfo.Num;
			Items[I]->BackpackItemInfo.LongRangeWeaponEquipAmmoNum = NewItemInfo.LongRangeWeaponEquipAmmoNum;
			break;
		}
	}
	// 向客户端反馈更新结果
	OnServerReportBackpackItemChanged.Broadcast(ClientUpdateTime);
}

void UBackpackComponent::SNC_UpdateItem(const FString& BackpackId, FBackpackItemInfo NewItemInfo)
{
	if (NewItemInfo.Num == 0)
	{
		// 当数量为 0 时，删除
		SNC_TryRemoveItem(BackpackId);
	}
	// 服务器作为客户端，直接修改数量，且更新 UI
	for (int I=0; I<Items.Num(); I++)
	{
		if (Items[I] && Items[I]->BackpackId == BackpackId)
		{
			Items[I]->BackpackItemInfo.Num = NewItemInfo.Num;
			Items[I]->BackpackItemInfo.LongRangeWeaponEquipAmmoNum = NewItemInfo.LongRangeWeaponEquipAmmoNum;
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
	if (BackpackWidget->IsVisible())
	{
		BackpackWidget->RemoveFromParent();

		PlayerController->bShowMouseCursor = false;
		FInputModeGameOnly GameOnly;
		
		PlayerController->SetInputMode(GameOnly);
		
	}
	else
	{
		 FInputModeGameAndUI GameAndUI;
		 if (BackpackWidget != nullptr)
		 {
			BackpackWidget->SetFocus();
		  BackpackWidget->SetKeyboardFocus();
		 	GameAndUI.SetWidgetToFocus(BackpackWidget->TakeWidget());
		 }
		 GameAndUI.SetHideCursorDuringCapture(false);

		 // SetLockMouseToViewportBehavior - 锁定鼠标在屏幕内的功能
		 GameAndUI.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

		// FInputModeUIOnly UIOnly;
		// if (BackpackWidget != nullptr)
		// {
		// 	BackpackWidget->SetFocus();
		// 	BackpackWidget->SetKeyboardFocus();
		// 	UIOnly.SetWidgetToFocus(BackpackWidget->TakeWidget());
		// }
		// UIOnly.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		
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
		UItemInfoObject* InBackpackItem = Items[Index];

		if (InBackpackItem==nullptr)
		{
			continue;
		}
		
		if (InBackpackItem->BackpackId.Equals(BackpackId))
		{
			// 只需要将数组元素置空，否则直接移除会有逻辑错误异常
			Items[Index] = nullptr;
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

void UBackpackComponent::CreateItemAfterDiscard(UItemInfoObject* InBackpackItemInfoObject)
{
	if (PlayerCharacter)
	{
		const FVector PlayerLocation = PlayerCharacter->GetActorLocation();
		
		if (InBackpackItemInfoObject->SceneItem != nullptr)
		{
			InBackpackItemInfoObject->SceneItem->PutInSceneHandle(PlayerLocation);
		}
	}
}

void UBackpackComponent::CC_CreateItemAfterDiscard_Implementation(AItemBase* Item)
{
	if (PlayerCharacter)
	{
		PlayerCharacter->GetCombatComponent()->OnBackpackThrowItem.Broadcast(Item->GetItemInfoObject());
	}
	if (PlayerController->HasAuthority())
	{
		SNC_CreateItemAfterDiscard(Item);
	}
	else
	{
		CreateItemAfterDiscard(Item->GetItemInfoObject());
		SC_CreateItemAfterDiscard(Item);
	}
}

void UBackpackComponent::SC_CreateItemAfterDiscard_Implementation(AItemBase* Item)
{
	if (PlayerCharacter)
	{
		PlayerCharacter->GetCombatComponent()->OnBackpackThrowItem.Broadcast(Item->GetItemInfoObject());
	}
	CreateItemAfterDiscard(Item->GetItemInfoObject());
}

void UBackpackComponent::SNC_CreateItemAfterDiscard(AItemBase* Item)
{
	if (PlayerCharacter)
	{
		PlayerCharacter->GetCombatComponent()->OnBackpackThrowItem.Broadcast(Item->GetItemInfoObject());
	}
	CreateItemAfterDiscard(Item->GetItemInfoObject());
}

void UBackpackComponent::GetItemsByType(EItemType InItemType, TArray<UItemInfoObject*>& InItems)
{
	for (auto Item : Items)
	{
		if (Item == nullptr)
		{
			continue;
		}
		
		if (Item->BackpackItemInfo.ItemType == InItemType)
		{
			InItems.Add(Item);
		}
	}
}

void UBackpackComponent::ServerReportBackpackDataOverride_Implementation(AItemBase* Item, FBackpackItemInfo BackpackItemInfo, int Index)
{
	// for (int I=0; I<Items.Num(); I++)
	// {
	// 	if (Items[I] && Items[I]->SceneItem == Item)
	// 	{
	// 		Items[I]->BackpackItemInfo = BackpackItemInfo;
	// 	}
	// }
	if (Item == nullptr)
	{
		return;
	}
	Items[Index] = Item->GetItemInfoObject();
	Items[Index]->BackpackItemInfo = BackpackItemInfo;
	OnBackpackItemChanged.Broadcast();
}

void UBackpackComponent::AddNewItem(UItemInfoObject* Item, int Index)
{
	const int X = Item->BackpackItemInfo.OccupyCellXYLength.X;
	const int Y = Item->BackpackItemInfo.OccupyCellXYLength.Y;
	Item->BackpackId = FString::Printf(TEXT("%d"), Index);
	
	for(int I=0; I < Y; I++)
	{
		int PlaceIndex = Index + Column * I;
		
		for(int J=0; J < X; J++)
		{
			const int TempJ = FMath::Min(1, J);
			PlaceIndex += TempJ;
			
			Items[PlaceIndex] = Item;
		}
	}
}
