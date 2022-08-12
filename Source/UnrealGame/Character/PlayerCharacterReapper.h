// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "UnrealGame/Enums/UnrealGameEnumInterface.h"
#include "PlayerCharacterReapper.generated.h"

struct FPlayerCharacterLagCompensationData;
struct FBoxData;
UCLASS()
class UNREALGAME_API AReapperCollisionBox : public AActor
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, Category="Reapper", DisplayName="Reapper Collision Box Component")
	UBoxComponent* CollisionBox;

	FName ReapperName;
	
public:	
	// Sets default values for this actor's properties
	AReapperCollisionBox();

	void UpdateReappeCollisionBox(FBoxData BoxData);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// TODO 碰撞处理
};

UCLASS()
class UNREALGAME_API APlayerCharacterReapper : public AActor
{
	GENERATED_BODY()

public:

	// 重现模式
	EReapperMode ReapperMode;

	// 处理临时重现的定时器
	FTimerHandle TemporaryReapperTimerHandle;

	// 处理序列重现的定时器
	FTimerHandle SequenceReapperTimerHandle;

	// 重现数据生成的对应碰撞盒子
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, DisplayName="Reapper Collision")
	TSubclassOf<AReapperCollisionBox> ReapperCollisionBoxClass;

	// 如果为 true，那么会一直获取玩家角色缓存的重现数据，循环重现，如果为 false，则重现当前序列的数据后停止
	UPROPERTY(BlueprintReadWrite, DisplayName="是否循环播放重现序列")
	bool bIsLoopPlaySequence;

	UPROPERTY(BlueprintReadWrite, DisplayName="重现序列帧间隔")
	float ReapperSequenceFrameInterval;
	
	// 临时重现的玩家角色滞后补偿数据
	TMap<FName, FBoxData> ReapperDataMap;

	// 序列重现的玩家角色滞后补偿数据
	TDoubleLinkedList<FPlayerCharacterLagCompensationData> ReapperSequenceDatas;

	// 玩家角色滞后补偿生成的碰撞盒子
	UPROPERTY()
	TMap<FName, AReapperCollisionBox*> ReapperCollisionBoxMap;

	// 序列重现所需的服务器重现时间
	float ServerReapperTime;
	
public:	
	// Sets default values for this actor's properties
	APlayerCharacterReapper();

	// 临时重现
	void TemporaryReapper(TMap<FName, FBoxData>& InReapperDataMap, float DelayDestroyTime, FName ReapperName);

	// 序列重现
	void SequenceReapper(TDoubleLinkedList<FPlayerCharacterLagCompensationData>& InReapperSequenceDatas, FName ReapperName, float FrameInterval = 0.1f, bool bIsLoop = false);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 销毁处理
	virtual void Destroyed() override;
	
private:

	// 初始化所有碰撞盒子
	void InitCollisionBoxs(FName ReapperName);

	// 延迟销毁定时器处理函数
	UFUNCTION()
	void DelayDestroy();

	// 序列重现定时器处理函数
	UFUNCTION()
	void SequenceReapperHandle();

	// 根据服务器时间，插值玩家角色滞后补偿数据
	// TMap<FName, FBoxData> DataInterporation(float ServerTime);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
