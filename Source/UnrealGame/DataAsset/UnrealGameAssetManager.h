// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "UnrealGameAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class UNREALGAME_API UUnrealGameAssetManager : public UAssetManager
{
	GENERATED_BODY()
public:
	// 定义资产类型
	inline static const FPrimaryAssetType LongRangeWeaponItemType = FPrimaryAssetType(FName("LongRangeWeapon"));
	inline static const FPrimaryAssetType ShotTextureItemType = FPrimaryAssetType(FName("ShotTexture"));

public:

	UFUNCTION(BlueprintCallable, Category="DataAssetManager", DisplayName="资产管理器")
	static UUnrealGameAssetManager* Get();

	UFUNCTION(BlueprintCallable, Category="DataAssetManager", DisplayName="强制加载资产")
	class UItemAsset* ForceLoadItem(const FPrimaryAssetId& PrimaryAssetId, bool bLogWarning = true);

	UFUNCTION(BlueprintCallable, Category="DataAssetManager", DisplayName="获取主资产ID列表")
	void K2_GetPrimaryAssetIdList(FPrimaryAssetType PrimaryAssetType, TArray<FPrimaryAssetId>& FPrimaryAssetIds);

	UFUNCTION(BlueprintCallable, Category="DataAssetManager", DisplayName="根据类型获取主资产类型所有对象")
	void K2_GetPrimaryAssetByType(FPrimaryAssetType PrimaryAssetType, TArray<UObject*>& ObjectList);

	UFUNCTION(BlueprintCallable, Category="DataAssetManager", DisplayName="根据Id获取主资产对象")
	UObject* K2_GetPrimaryAssetById(FPrimaryAssetId PrimaryAssetId);

	UFUNCTION(BlueprintCallable, Category="DataAssetManager", DisplayName="根据资产Id加载主资产")
	void K2_LoadPrimaryAssetById(FPrimaryAssetId PrimaryAssetId);

	
private:
	
};
