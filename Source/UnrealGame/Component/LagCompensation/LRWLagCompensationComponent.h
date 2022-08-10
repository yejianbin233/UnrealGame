// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LRWLagCompensationComponent.generated.h"

DECLARE_EVENT_OneParam(ALongRangeWeapon, FOnLoadAmmoChanged, float/*AmmoChangedTime*/)
DECLARE_EVENT_TwoParams(ALongRangeWeapon, FOnServerLoadAmmoChangedFeedback, float/*AmmoChangedTime*/, float /*AmmoNum*/)

/*
 * ULRWLagCompensationComponent - LongRangeWeapon 的 LagCompensationComponent
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALGAME_API ULRWLagCompensationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULRWLagCompensationComponent();

	TDoubleLinkedList<float> LoadAmmomChangedDatas;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(Category="Ammo", DisplayName="缓存子弹改变数据")
	void CacheLoadAmmoChangedData(float ChangedTime);

	UFUNCTION(Client, Reliable, Category="Ammo", DisplayName="缓存子弹改变数据")
	void ServerReportLoadAmmoChangedResult(float AmmoChangedTime, float AmmoNum);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


	
};
