// Fill out your copyright notice in the Description page of Project Settings.


#include "BackpackLagCompensationComponent.h"

#include "BackpackComponent.h"
#include "ItemBase.h"
#include "Kismet/GameplayStatics.h"
#include "UnrealGame/Character/BlasterCharacter.h"

// Sets default values for this component's properties
UBackpackLagCompensationComponent::UBackpackLagCompensationComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

// Called when the game starts
void UBackpackLagCompensationComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	PlayerCharacter = Cast<ABlasterCharacter>(GetOwner());

	if (PlayerCharacter)
	{
		UBackpackComponent* BackpackComponent = PlayerCharacter->GetBackpackComponent();
		if (BackpackComponent)
		{
			if (PlayerCharacter->GetLocalRole() == ROLE_AutonomousProxy)
			{
				// 客户端绑定，当服务器作为客户端时，不会绑定
				BackpackComponent->OnClientBackpackItemChanged.AddUObject(this, &UBackpackLagCompensationComponent::CacheBackpackItemChangedData);
			}

			if (PlayerCharacter->GetLocalRole() == ROLE_Authority)
			{
				// 服务器绑定
				BackpackComponent->OnServerReportBackpackItemChanged.AddUObject(this, &UBackpackLagCompensationComponent::ServerFeedbackBackpackItemChangedResult);
			}
		}
	}
}


void UBackpackLagCompensationComponent::ServerFeedbackPickupItemFailture_Implementation(AItemBase* PickupFailtureItem)
{
	if (PickupFailtureItem != nullptr)
	{
		// 只要服务器添加失败，都在客户端执行，不改变位置
		PickupFailtureItem->PutInSceneHandle(PickupFailtureItem->GetActorLocation());
	}
}

void UBackpackLagCompensationComponent::CacheBackpackItemChangedData_Implementation(float BackpackItemChangedTime)
{
	BackpacItemChangedDates.AddTail(BackpackItemChangedTime);
}

void UBackpackLagCompensationComponent::ServerFeedbackBackpackItemChangedResult_Implementation(
	float BackpackItemChangedTime)
{
	TDoubleLinkedList<float>::TDoubleLinkedListNode* TailNode = BackpacItemChangedDates.GetTail();
	if (TailNode)
	{
		float ChangedTime = TailNode->GetValue();
		if (BackpackItemChangedTime >= ChangedTime)
		{
			ClientRequestServerBackpackDataOverride();

			TDoubleLinkedList<float>::TDoubleLinkedListNode* PrevNode = TailNode->GetPrevNode();
			// 将当前节点之前的所有节点移除
			while (PrevNode)
			{
				TDoubleLinkedList<float>::TDoubleLinkedListNode* NextPrevNode = PrevNode->GetPrevNode();
				BackpacItemChangedDates.RemoveNode(PrevNode);
				PrevNode = NextPrevNode;
			}
		}
	}
	else
	{
		// 如果客户端没有缓存数据，那么说明服务器添加了背包数据，请求更新
		ClientRequestServerBackpackDataOverride();
	}
}

void UBackpackLagCompensationComponent::ClientRequestServerBackpackDataOverride_Implementation()
{
	// 如果允许服务器覆盖当前客户端的背包数据，则请求覆盖
	if (PlayerCharacter->GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	
	if (PlayerCharacter)
	{
		UBackpackComponent* BackpackComponent = PlayerCharacter->GetBackpackComponent();
		if (BackpackComponent)
		{

			TArray<UItemInfoObject*> Items = BackpackComponent->GetBackpackItemDatas();
			for (int I=0; I < Items.Num(); I++)
			{
				if (Items[I])
				{
					ServerReportBackpackData(Items[I]->SceneItem, Items[I]->BackpackItemInfo, I);
				}
				else
				{
					ServerReportBackpackData(nullptr, FBackpackItemInfo(), I);
				}
			}
		}
	}
}

void UBackpackLagCompensationComponent::ServerReportBackpackData_Implementation(
	AItemBase* Item, FBackpackItemInfo BackpackItemInfo, int Index)
{
	if (PlayerCharacter->GetLocalRole() != ROLE_AutonomousProxy)
	{
		return;
	}
	// 服务器作为客户端不需要反馈更新
	OnServerReportBackpackDataOverride.Broadcast(Item, BackpackItemInfo, Index);
}



