// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemAsset.h"

FPrimaryAssetId UItemAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(ItemType, ItemName);
}
