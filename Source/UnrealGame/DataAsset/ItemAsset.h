// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemAsset.generated.h"

/**
 * 
 */
UCLASS()
class UNREALGAME_API UItemAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	// 资产类型
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DataAsset Property")
	FPrimaryAssetType ItemType;

	// 资产名称
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DataAsset Property")
	FName ItemName;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
