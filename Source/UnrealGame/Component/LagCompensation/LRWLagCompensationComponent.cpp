// Fill out your copyright notice in the Description page of Project Settings.


#include "LRWLagCompensationComponent.h"

#include "UnrealGame/Weapon/LongRangeWeapon/LongRangeWeapon.h"

// Sets default values for this component's properties
ULRWLagCompensationComponent::ULRWLagCompensationComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void ULRWLagCompensationComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	ALongRangeWeapon* Owner = Cast<ALongRangeWeapon>(GetOwner());
	if (Owner)
	{
		if (!GetOwner()->HasAuthority())
		{
			Owner->OnLoadAmmoChanged.AddUObject(this, &ThisClass::CacheLoadAmmoChangedData);
		}
	}
}


// Called every frame
void ULRWLagCompensationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void ULRWLagCompensationComponent::CacheLoadAmmoChangedData(float ChangedTime)
{
	LoadAmmomChangedDatas.AddTail(ChangedTime);
}

void ULRWLagCompensationComponent::ServerReportLoadAmmoChangedResult_Implementation(float AmmoChangedTime, float AmmoNum)
{
	TDoubleLinkedList<float>::TDoubleLinkedListNode* TailNode = LoadAmmomChangedDatas.GetTail();
	if (TailNode)
	{
		float ChangedTime = TailNode->GetValue();
		if (AmmoChangedTime >= ChangedTime)
		{
			ALongRangeWeapon* LongRangeWeapon = Cast<ALongRangeWeapon>(GetOwner());

			if (LongRangeWeapon)
			{
				LongRangeWeapon->LoadAmmo = AmmoNum;
			}
			TDoubleLinkedList<float>::TDoubleLinkedListNode* PrevNode = TailNode->GetPrevNode();
			// 将当前节点之前的所有节点移除
			while (PrevNode)
			{
				TDoubleLinkedList<float>::TDoubleLinkedListNode* NextPrevNode = PrevNode->GetPrevNode();
				LoadAmmomChangedDatas.RemoveNode(PrevNode);
				PrevNode = NextPrevNode;
			}
		}
	}
	else
	{
		ALongRangeWeapon* LongRangeWeapon = Cast<ALongRangeWeapon>(GetOwner());
		if (LongRangeWeapon)
		{
			LongRangeWeapon->LoadAmmo = AmmoNum;
		}
	}
}

