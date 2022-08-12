// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacterReapper.h"

#include "BlasterCharacter.h"
#include "UnrealGame/Component/LagCompensation/PlayerLagCompensationComponent.h"
#include "UnrealGame/Struct/UnrealGameStruct.h"

AReapperCollisionBox::AReapperCollisionBox()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));

	CollisionBox->SetupAttachment(GetRootComponent());
}

void AReapperCollisionBox::UpdateReappeCollisionBox(FBoxData BoxData)
{
	this->SetActorLocation(BoxData.Location);
	CollisionBox->SetWorldRotation(BoxData.Rotator);
	CollisionBox->SetBoxExtent(BoxData.ScaledExtent);
}

void AReapperCollisionBox::BeginPlay()
{
	Super::BeginPlay();
}

// Sets default values
APlayerCharacterReapper::APlayerCharacterReapper()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called every frame
void APlayerCharacterReapper::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called when the game starts or when spawned
void APlayerCharacterReapper::BeginPlay()
{
	Super::BeginPlay();
	
}

void APlayerCharacterReapper::Destroyed()
{
	Super::Destroyed();
}

void APlayerCharacterReapper::TemporaryReapper(TMap<FName, FBoxData>& InReapperDataMap, float DelayDestroyTime, FName ReapperName)
{
	ReapperDataMap.Empty();
	ReapperDataMap.Append(InReapperDataMap);

	ReapperMode = EReapperMode::Temporary;
	InitCollisionBoxs(ReapperName);
	
	// 定时销毁
	GetWorld()->GetTimerManager().SetTimer(TemporaryReapperTimerHandle, this, &APlayerCharacterReapper::DelayDestroy, DelayDestroyTime, false);
}

void APlayerCharacterReapper::SequenceReapper(
	TDoubleLinkedList<FPlayerCharacterLagCompensationData>& InReapperSequenceDatas, FName ReapperName, float FrameInterval, bool bIsLoop)
{
	ReapperSequenceDatas.Empty();
	TDoubleLinkedList<FPlayerCharacterLagCompensationData>::TDoubleLinkedListNode* HeadNode = InReapperSequenceDatas.GetHead();
	while (HeadNode)
	{
		FPlayerCharacterLagCompensationData Data;
		Data.ServerCacheTime = HeadNode->GetValue().ServerCacheTime;
		Data.PlayerCharacterBoneCollisionBoxMap.Append(HeadNode->GetValue().PlayerCharacterBoneCollisionBoxMap);
		ReapperSequenceDatas.AddTail(Data);
		HeadNode = HeadNode->GetNextNode();
	}
	bIsLoopPlaySequence = bIsLoop;
	ReapperSequenceFrameInterval = FrameInterval;
	ServerReapperTime = ReapperSequenceDatas.GetHead() == nullptr ? 0 : ReapperSequenceDatas.GetHead()->GetValue().ServerCacheTime;
	ReapperMode = EReapperMode::Sequence;
	InitCollisionBoxs(ReapperName);
	
	GetWorld()->GetTimerManager().SetTimer(SequenceReapperTimerHandle, this, &APlayerCharacterReapper::SequenceReapperHandle, ReapperSequenceFrameInterval, true);
}

void APlayerCharacterReapper::InitCollisionBoxs(FName ReapperName)
{
	if (ReapperMode == EReapperMode::Temporary)
	{
		if (ReapperDataMap.Num() > 0)
		{
			for (auto ReapperData : ReapperDataMap)
			{
				FRotator BoxRotator(0,0,0);
				FActorSpawnParameters SpawnParameters;
				AReapperCollisionBox* TempReapperCollisionBox = GetWorld()->SpawnActor<AReapperCollisionBox>(ReapperCollisionBoxClass, ReapperData.Value.Location, BoxRotator, SpawnParameters);

				TempReapperCollisionBox->ReapperName = ReapperName;
				TempReapperCollisionBox->CollisionBox->SetWorldRotation(ReapperData.Value.Rotator);
				TempReapperCollisionBox->CollisionBox->SetBoxExtent(ReapperData.Value.ScaledExtent);

				ReapperCollisionBoxMap.Add(ReapperData.Key, TempReapperCollisionBox);
			}
		}
	}
	else if (ReapperMode == EReapperMode::Sequence)
	{
		
		if (ReapperSequenceDatas.Num() > 0 && ReapperSequenceDatas.GetHead()->GetValue().PlayerCharacterBoneCollisionBoxMap.Num() > 0)
		{
			for (auto ReapperData : ReapperSequenceDatas.GetHead()->GetValue().PlayerCharacterBoneCollisionBoxMap)
			{
				FRotator BoxRotator(0,0,0);
				FActorSpawnParameters SpawnParameters;
				AReapperCollisionBox* TempReapperCollisionBox = GetWorld()->SpawnActor<AReapperCollisionBox>(ReapperCollisionBoxClass, ReapperData.Value.Location, BoxRotator, SpawnParameters);

				TempReapperCollisionBox->ReapperName = ReapperName;
				TempReapperCollisionBox->CollisionBox->SetWorldRotation(ReapperData.Value.Rotator);
				TempReapperCollisionBox->CollisionBox->SetBoxExtent(ReapperData.Value.ScaledExtent);

				ReapperCollisionBoxMap.Add(ReapperData.Key, TempReapperCollisionBox);
			}
		}
	}
	
}

void APlayerCharacterReapper::DelayDestroy()
{
	for (auto Item : ReapperCollisionBoxMap)
	{
		if (Item.Value)
		{
			Item.Value->Destroy();
		}
	}
	
	bool DestryResult = this->Destroy(true);
	if (!DestryResult)
	{
		UE_LOG(LogTemp, Warning, TEXT("Reapper Destroy Fail!"));
	}
}

void APlayerCharacterReapper::SequenceReapperHandle()
{
	if (!bIsLoopPlaySequence)
	{
		// 终止
		GetWorld()->GetTimerManager().ClearTimer(SequenceReapperTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(TemporaryReapperTimerHandle, this, &APlayerCharacterReapper::DelayDestroy, ReapperSequenceFrameInterval, false);
	}
	
	ServerReapperTime += ReapperSequenceFrameInterval;
	ABlasterCharacter* PlayerCharacter = Cast<ABlasterCharacter>(GetOwner());
	if (PlayerCharacter)
	{
		
		UPlayerLagCompensationComponent* PlayerLagCompensationComponent = PlayerCharacter->GetPlayerLagCompensationComponent();

		TMap<FName, FBoxData> InterporationDataMap = UPlayerLagCompensationComponent::DataInterporation(ReapperSequenceDatas, ServerReapperTime);
		if (InterporationDataMap.Num() > 0)
		{
			for (auto BoxItem : InterporationDataMap)
			{
				AReapperCollisionBox* CollisionBoxComponent = *ReapperCollisionBoxMap.Find(BoxItem.Key);
				if (CollisionBoxComponent)
				{
					CollisionBoxComponent->UpdateReappeCollisionBox(BoxItem.Value);
				}
			}
		}
		else
		{
			// 如果没有可重现的数据，那么根据 bIsLoopPlaySequence 重新请求新的数据，如果确定没有重现数据，则销毁。
			if (bIsLoopPlaySequence)
			{
				ReapperSequenceDatas.Empty();
				PlayerLagCompensationComponent->GetSequenceReapperDatas(ReapperSequenceDatas, ServerReapperTime);
				TMap<FName, FBoxData> AgaintInterporationDataMap = UPlayerLagCompensationComponent::DataInterporation(ReapperSequenceDatas, ServerReapperTime);
				if (AgaintInterporationDataMap.Num() > 0)
				{
					for (auto BoxItem : InterporationDataMap)
					{
						AReapperCollisionBox* CollisionBoxComponent = *ReapperCollisionBoxMap.Find(BoxItem.Key);
						if (CollisionBoxComponent)
						{
							CollisionBoxComponent->UpdateReappeCollisionBox(BoxItem.Value);
						}
					}
				}
				else
				{
					// 终止
					GetWorld()->GetTimerManager().ClearTimer(SequenceReapperTimerHandle);
					GetWorld()->GetTimerManager().SetTimer(TemporaryReapperTimerHandle, this, &APlayerCharacterReapper::DelayDestroy, ReapperSequenceFrameInterval, false);
				}
			}
		}
	}
}
