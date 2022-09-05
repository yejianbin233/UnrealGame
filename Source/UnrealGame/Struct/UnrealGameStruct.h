// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UnrealGame/Enums/UnrealGameEnumInterface.h"
#include "UnrealGameStruct.generated.h"

class UItemInfoObject;
/**
 * 
 */
UCLASS()
class UNREALGAME_API UUnrealGameStruct : public UObject
{
	GENERATED_BODY()
	
};

// USTRUCT(BlueprintType)
// struct FItemUseData : public FTableRowBase
// {
// 	GENERATED_BODY()
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="子弹类")
// 	TSubclassOf<class AProjectile> ProjectileClass;
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="物品使用类")
// 	TSubclassOf<class AItemAbility> AItemAbility;
// };

USTRUCT(BlueprintType)
struct FItemUseInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="场景物品拾取物品类")
	TSubclassOf<class AItemBase> ItemBase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="物品使用功能类")
	TSubclassOf<class AItemUse> AItemUse;
};

USTRUCT(BlueprintType)
struct FProjectileData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="子弹类")
	TSubclassOf<class AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="弹壳类")
	TSubclassOf<class ACasing> CasingClass;
};



USTRUCT()
struct FBoxData
{
	GENERATED_BODY()
	
	FVector Location;
	FRotator Rotator;
	FVector ScaledExtent;
};

USTRUCT(BlueprintType)
struct FPickupResult
{
	GENERATED_BODY()

	FPickupResult(): bIsNoSpace(false), Result(), ItemInfoObject(nullptr)
	{
	};
	
	FPickupResult(EPickupResult InResult, UItemInfoObject* InItemInfoObject, bool InIsNoSpace)
	{
		this->Result = InResult;
		this->ItemInfoObject = InItemInfoObject;
		this->bIsNoSpace = InIsNoSpace;
	}

	bool bIsNoSpace;
	
	UPROPERTY(BlueprintReadWrite, Category="Pickup", DisplayName="拾取结果")
	EPickupResult Result;

	UPROPERTY(BlueprintReadWrite, Category="Pickup", DisplayName="背包物品数据对象")
	UItemInfoObject* ItemInfoObject;
};


