// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UnrealGame/Enums/UnrealGameEnumInterface.h"
#include "UnrealGameStruct.generated.h"

/**
 * 
 */
UCLASS()
class UNREALGAME_API UUnrealGameStruct : public UObject
{
	GENERATED_BODY()
	
};

USTRUCT(BlueprintType)
struct FEquipmentInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="物品类")
	TSubclassOf<class AItemBase> ItemBase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="装备类")
	TSubclassOf<class AWeapon> EquipmentBase;
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

USTRUCT(BlueprintType)
struct FPickupObjectData
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, DisplayName="可拾取Actor")
	AActor* PickableActor;

	UPROPERTY(BlueprintReadOnly, DisplayName="处理状态")
	EPickableObjectState HandleState;

	UPROPERTY(BlueprintReadOnly, DisplayName="处理状态")
	EPickableObjectState TargetState;
};

