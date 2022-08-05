// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealGameAssetManager.h"
#include "ItemAsset.h"

UUnrealGameAssetManager* UUnrealGameAssetManager::Get()
{
	UUnrealGameAssetManager* This = Cast<UUnrealGameAssetManager>(GEngine->AssetManager);

	if (This)
	{
		return This;
	}
	else
	{
		UE_LOG(LogTemp, Fatal, TEXT("Invalid AssetManager in DefaultEngine.ini, must be RPGAssetManager!"));
		return NewObject<UUnrealGameAssetManager>(); // never calls this
	}
}

UItemAsset* UUnrealGameAssetManager::ForceLoadItem(const FPrimaryAssetId& PrimaryAssetId, bool bLogWarning)
{
	FSoftObjectPath ItemPath = GetPrimaryAssetPath(PrimaryAssetId);

	// This does a synchronous load and may hitch
	UItemAsset* LoadedItem = Cast<UItemAsset>(ItemPath.TryLoad());

	if (bLogWarning && LoadedItem == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load item for identifier %s!"), *PrimaryAssetId.ToString());
	}

	return LoadedItem;
}

void UUnrealGameAssetManager::K2_GetPrimaryAssetIdList(FPrimaryAssetType PrimaryAssetType,
	TArray<FPrimaryAssetId>& FPrimaryAssetIds)
{
	GetPrimaryAssetIdList(PrimaryAssetType, FPrimaryAssetIds);
}

void UUnrealGameAssetManager::K2_GetPrimaryAssetByType(FPrimaryAssetType PrimaryAssetType, TArray<UObject*>& ObjectList)
{
	GetPrimaryAssetObjectList(PrimaryAssetType, ObjectList);
}

UObject* UUnrealGameAssetManager::K2_GetPrimaryAssetById(FPrimaryAssetId PrimaryAssetId)
{
	return GetPrimaryAssetObject(PrimaryAssetId);
}

void UUnrealGameAssetManager::K2_LoadPrimaryAssetById(FPrimaryAssetId PrimaryAssetId)
{
	LoadPrimaryAsset(PrimaryAssetId);
}

