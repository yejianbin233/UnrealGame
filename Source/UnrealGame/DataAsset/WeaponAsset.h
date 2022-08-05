// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemAsset.h"
#include "../Enums/UnrealGameEnumInterface.h"
#include "WeaponAsset.generated.h"

/**
 * 
 */
UCLASS()
class UNREALGAME_API UWeaponAsset : public UItemAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon", DisplayName="武器网格体")
	USkeletalMesh* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon", DisplayName="武器动画实例")
	TSubclassOf<UAnimInstance> AnimInstanceClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon", DisplayName="武器插槽名称")
	FName EquippedWeaponSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Equipment", DisplayName="装备武器蒙太奇动画")
	UAnimMontage* EquippedMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon", DisplayName="玩家装备武器后的状态")
	EPlayerEquipState PlayerEquipState;
};
