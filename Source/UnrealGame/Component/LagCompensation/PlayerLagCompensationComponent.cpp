// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerLagCompensationComponent.h"

#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "UnrealGame/Character/BlasterCharacter.h"
#include "UnrealGame/Character/PlayerCharacterReapper.h"

// Sets default values for this component's properties
UPlayerLagCompensationComponent::UPlayerLagCompensationComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

// Called when the game starts
void UPlayerLagCompensationComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (GetOwner()->HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(PlayerCharacterLagCompensationTimerHandle, this, &UPlayerLagCompensationComponent::CachePlayerCharacterLagCompensationData, CacheInterval, true);
		GetWorld()->GetTimerManager().SetTimer(ExpiredCacheDataClearTimerHandle, this, &UPlayerLagCompensationComponent::ExpiredCacheDataClear, CacheClearInterval, true);
	}
}

void UPlayerLagCompensationComponent::ReapperPlayerCharacterInTimePoint(float ServerTime, float ReapperDuration, FName ReapperName)
{
	FVector Location(0,0,0);
	FRotator Rotator(0,0,0);
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = GetOwner();
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	APlayerCharacterReapper* PlayerCharacterReapper = GetWorld()->SpawnActor<APlayerCharacterReapper>(PlayerCharacterReapperClass, Location, Rotator, SpawnParameters);
	if (PlayerCharacterReapper)
	{
		TMap<FName, FBoxData> TemporaryReapperDataMap = DataInterporation(PlayerCharacterLagCompensationDatas, ServerTime);
		PlayerCharacterReapper->TemporaryReapper(TemporaryReapperDataMap, ReapperDuration, ReapperName);
	}
}

void UPlayerLagCompensationComponent::ReapperPlayerCharacterSequenceFromTimePointStart(float ServerTime, FName ReapperName)
{
	FVector Location(0,0,0);
	FRotator Rotator(0,0,0);
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = GetOwner();
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	APlayerCharacterReapper* PlayerCharacterReapper = GetWorld()->SpawnActor<APlayerCharacterReapper>(PlayerCharacterReapperClass, Location, Rotator, SpawnParameters);

	if (PlayerCharacterReapper)
	{
		TDoubleLinkedList<FPlayerCharacterLagCompensationData> SequenceReapperDatas;
		GetSequenceReapperDatas(SequenceReapperDatas, ServerTime);
	
		PlayerCharacterReapper->SequenceReapper(SequenceReapperDatas, ReapperName, ReapperSequenceFrame,  true);
	}
}

void UPlayerLagCompensationComponent::GetSequenceReapperDatas(
	TDoubleLinkedList<FPlayerCharacterLagCompensationData>& SequenceReapperDatas, float ServerReapperTime)
{
	TDoubleLinkedList<FPlayerCharacterLagCompensationData>::TDoubleLinkedListNode* HeadNode = PlayerCharacterLagCompensationDatas.GetHead();
	
	if (HeadNode)
	{
		TDoubleLinkedList<FPlayerCharacterLagCompensationData>::TDoubleLinkedListNode* IteraterNode = HeadNode;
		while (IteraterNode->GetValue().ServerCacheTime < ServerReapperTime)
		{
			IteraterNode = IteraterNode->GetNextNode();
		}

		if (IteraterNode)
		{
			if (IteraterNode->GetValue().ServerCacheTime == ServerReapperTime)
			{
				SequenceReapperDatas.AddTail(IteraterNode);
			}
			else
			{
				FPlayerCharacterLagCompensationData ServerTimeReapperData;
				ServerTimeReapperData.ServerCacheTime = ServerReapperTime;
				// 计算 ServerReapperTime 的插值数据
				ServerTimeReapperData.PlayerCharacterBoneCollisionBoxMap = DataInterporation(IteraterNode->GetPrevNode()->GetValue(), IteraterNode->GetNextNode()->GetValue(), ServerReapperTime);
				SequenceReapperDatas.AddTail(ServerTimeReapperData);
			}

			IteraterNode = IteraterNode->GetNextNode();
			while (IteraterNode)
			{
				SequenceReapperDatas.AddTail(IteraterNode->GetValue());
				IteraterNode = IteraterNode->GetNextNode();
			}
		}
	}
}

void UPlayerLagCompensationComponent::CachePlayerCharacterLagCompensationData()
{
	ABlasterCharacter* PlayerCharacter = Cast<ABlasterCharacter>(GetOwner());

	TArray<UActorComponent*> BoneCollisionBoxComponents = PlayerCharacter->GetComponentsByTag(UBoxComponent::StaticClass(), PlayerCharacter->GetBoneCollisionBoxTagName());

	float ServerCacheTime = GetWorld()->GetTimeSeconds();
	
	if (BoneCollisionBoxComponents.Num() > 0)
	{

		TMap<FName, FBoxData> BoxDataMap;
		for (auto BoneCollisionBoxComponent : BoneCollisionBoxComponents)
		{
			UBoxComponent* TempBoxComponent = Cast<UBoxComponent>(BoneCollisionBoxComponent);
			if (TempBoxComponent)
			{
				FBoxData BoxData;
				BoxData.Location = TempBoxComponent->GetComponentLocation();
				BoxData.Rotator = TempBoxComponent->GetComponentRotation();
				BoxData.ScaledExtent = TempBoxComponent->GetScaledBoxExtent();
				BoxDataMap.Add(*TempBoxComponent->GetName(), BoxData);
			}
		}
		FPlayerCharacterLagCompensationData CacheData;
		CacheData.ServerCacheTime = ServerCacheTime;
		CacheData.PlayerCharacterBoneCollisionBoxMap = BoxDataMap;
		PlayerCharacterLagCompensationDatas.AddTail(CacheData);
	}
}

void UPlayerLagCompensationComponent::ExpiredCacheDataClear()
{
	float ServerTime = GetWorld()->GetTimeSeconds();
	
	TDoubleLinkedList<FPlayerCharacterLagCompensationData>::TDoubleLinkedListNode* HeadNode = PlayerCharacterLagCompensationDatas.GetHead();
	if (HeadNode)
	{
		float TimeInterval = FMath::Abs(ServerTime - HeadNode->GetValue().ServerCacheTime);
		while (TimeInterval > MaxCacheTime)
		{
			TDoubleLinkedList<FPlayerCharacterLagCompensationData>::TDoubleLinkedListNode* NextNode = HeadNode->GetNextNode();
			PlayerCharacterLagCompensationDatas.RemoveNode(HeadNode);
			HeadNode = NextNode;

			if (HeadNode == nullptr)
			{
				break;
			}
			TimeInterval = FMath::Abs(ServerTime - HeadNode->GetValue().ServerCacheTime);
		}
	}
}

TMap<FName, FBoxData> UPlayerLagCompensationComponent::DataInterporation(FPlayerCharacterLagCompensationData PreData,
	FPlayerCharacterLagCompensationData NextData, float ServerTime)
{
	TMap<FName, FBoxData> InterporationDataMap;
	TMap<FName, FBoxData> PreNodeDataMap = PreData.PlayerCharacterBoneCollisionBoxMap;
	TMap<FName, FBoxData> NextNodeDataMap = NextData.PlayerCharacterBoneCollisionBoxMap;

	float InterporationValue = UKismetMathLibrary::MapRangeClamped(ServerTime
		, PreData.ServerCacheTime
		, NextData.ServerCacheTime
		, 0
		, 1);
		
	for (auto MapItem : PreNodeDataMap)
	{
		FName BoneName = MapItem.Key;
		FBoxData PreBoxData = MapItem.Get<FBoxData>();
		FBoxData* NextBoxData = NextNodeDataMap.Find(BoneName);
		if (NextBoxData)
		{
			FBoxData InterporationData;
			InterporationData.Location = UKismetMathLibrary::VLerp(PreBoxData.Location, NextBoxData->Location, InterporationValue);
			InterporationData.Rotator = UKismetMathLibrary::RLerp(PreBoxData.Rotator, NextBoxData->Rotator, InterporationValue, true);
			InterporationData.ScaledExtent = UKismetMathLibrary::VLerp(PreBoxData.ScaledExtent, NextBoxData->ScaledExtent, InterporationValue);;
			InterporationDataMap.Add(BoneName, InterporationData);
		}
	}

	return InterporationDataMap;
}

TMap<FName, FBoxData> UPlayerLagCompensationComponent::DataInterporation(TDoubleLinkedList<FPlayerCharacterLagCompensationData>& SequenceReapperDatas, float ServerTime)
{
	TDoubleLinkedList<FPlayerCharacterLagCompensationData>::TDoubleLinkedListNode* HeadNode = SequenceReapperDatas.GetHead();

	TDoubleLinkedList<FPlayerCharacterLagCompensationData>::TDoubleLinkedListNode* ServerTimePreNode = nullptr;
	TDoubleLinkedList<FPlayerCharacterLagCompensationData>::TDoubleLinkedListNode* ServerTimeNextNode = nullptr;
	
	if (HeadNode)
	{
		TDoubleLinkedList<FPlayerCharacterLagCompensationData>::TDoubleLinkedListNode* IteraterNode = HeadNode;
		while (IteraterNode && IteraterNode->GetValue().ServerCacheTime < ServerTime)
		{
			IteraterNode = IteraterNode->GetNextNode();
		}

		if (IteraterNode)
		{
			if (IteraterNode->GetValue().ServerCacheTime == ServerTime)
			{
				// 不需要进行插值
				return IteraterNode->GetValue().PlayerCharacterBoneCollisionBoxMap;
			}
			else
			{
				ServerTimePreNode = IteraterNode->GetPrevNode();
				ServerTimeNextNode = IteraterNode->GetNextNode();
			}
		}
	}
	
	if (ServerTimePreNode && ServerTimeNextNode)
	{
		return DataInterporation(ServerTimePreNode->GetValue(), ServerTimeNextNode->GetValue(), ServerTime);
	}
	
	TMap<FName, FBoxData> InterporationDataMap;
	return InterporationDataMap;
}
