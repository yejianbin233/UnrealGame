// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Containers/List.h"
#include "UnrealGame/Struct/UnrealGameStruct.h"
#include "PlayerLagCompensationComponent.generated.h"

USTRUCT()
struct FPlayerCharacterLagCompensationData
{
	GENERATED_BODY()

	UPROPERTY(DisplayName="服务器缓存时间")
	float ServerCacheTime;

	UPROPERTY(DisplayName="玩家角色骨骼碰撞盒子数据")
	TMap<FName, FBoxData> PlayerCharacterBoneCollisionBoxMap;
};

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALGAME_API UPlayerLagCompensationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerLagCompensationComponent();

	// 处理玩家角色滞后补偿数据的时间管理器
	FTimerHandle PlayerCharacterLagCompensationTimerHandle;
	// 清除玩家角色过期的滞后补偿数据的时间管理器
	FTimerHandle ExpiredCacheDataClearTimerHandle;

	// 缓存重现数据的频率
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Lag Compensation", DisplayName="缓存间隔")
	float CacheInterval;

	// 重现数据缓存清除频率
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Lag Compensation", DisplayName="缓存数据清除间隔")
	float CacheClearInterval;

	// 当超过最大缓存时间时，清除重现数据
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Lag Compensation", DisplayName="最大缓存时间")
	float MaxCacheTime;

	/*
	 *  帧数频率，值越小越快，重现越流畅。
	 *  note：用于定时器，若生效不可为 0
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Lag Compensation", DisplayName="重现序列帧频率", meta=(UIMin=0, UIMax=1))
	float ReapperSequenceFrame = 0.1f;

	// 用于出现重现数据
	UPROPERTY(EditDefaultsOnly, Category="Lag Compensation", DisplayName="玩家数据重现类")
	TSubclassOf<class APlayerCharacterReapper> PlayerCharacterReapperClass;
	
public:
	
	UFUNCTION(BlueprintCallable, Category="Player Character Lag Compensation", DisplayName="重现某个时间点的玩家角色并持续指定时间")
	void ReapperPlayerCharacterInTimePoint(float ServerTime, float ReapperDuration, FName ReapperName);

	UFUNCTION(BlueprintCallable, Category="Player Character Lag Compensation", DisplayName="从指定时间点开始重现玩家角色移动序列")
	void ReapperPlayerCharacterSequenceFromTimePointStart(float ServerTime, FName ReapperName);
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
private:

	// 缓存的玩家角色滞后补偿数据
	TDoubleLinkedList<FPlayerCharacterLagCompensationData> PlayerCharacterLagCompensationDatas;

	UFUNCTION(Category="Player Character Lag Compensation", DisplayName="缓存玩家角色滞后补偿数据")
	void CachePlayerCharacterLagCompensationData();

	UFUNCTION(Category="Player Character Lag Compensation", DisplayName="清除过期的玩家角色滞后补偿数据")
	void ExpiredCacheDataClear();

	// 根据两个时间点之间的数据进行线性插值(Alpha)
	static TMap<FName, FBoxData> DataInterporation(FPlayerCharacterLagCompensationData PreData, FPlayerCharacterLagCompensationData NextData, float ServerTime);

public:
	// 根据服务器时间，插值玩家角色滞后补偿数据
	// static TMap<FName, FBoxData> DataInterporation(FPlayerCharacterLagCompensationData PreData, FPlayerCharacterLagCompensationData NextData, float ServerTime);

	// 在线性时间数据链表中进行线性插值(Alpha)
	static TMap<FName, FBoxData> DataInterporation(TDoubleLinkedList<FPlayerCharacterLagCompensationData>& SequenceReapperDatas, float ServerTime);
	
	// 获取指定服务器时间点只有的玩家角色滞后补偿数据
	void GetSequenceReapperDatas(TDoubleLinkedList<FPlayerCharacterLagCompensationData>& SequenceReapperDatas, float ServerReapperTime);
};
