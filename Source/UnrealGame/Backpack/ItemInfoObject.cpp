// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemInfoObject.h"

#include "ItemBase.h"
#include "Net/UnrealNetwork.h"

void UItemInfoObject::Init(AItemBase* Item)
{
	
	this->BackpackItemInfo = Item->GetItemInfo();
	this->SceneItem = Item;

	if (Item->GetItemUseClass())
	{
		if (UWorld* World = Item->GetWorld())
		{
			this->UseItem = World->SpawnActor<AItemUse>(Item->GetItemUseClass());
		}
	}
}

void UItemInfoObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UItemInfoObject, BackpackId);
}
