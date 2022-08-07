// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UnrealGame/Enums/UnrealGameEnumInterface.h"
#include "UnrealGame/Enums/WeaponTypes.h"
#include "Weapon.generated.h"

UCLASS()
class UNREALGAME_API AWeapon : public AActor
{
	GENERATED_BODY()

	friend class ABlasterCharacter;
	friend class UCombatComponent;
	
protected:
	
	UPROPERTY(Replicated, BlueprintReadOnly, Category="Reference", DisplayName="玩家角色", meta=(AllowPrivateAccess))
	class ABlasterCharacter* PlayerCharacter;

	// 枪骨骼网格体
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon", DisplayName="武器网格体", meta=(AllowPrivateAccess))
	class USkeletalMeshComponent* WeaponMeshComponent;

	// 资产数据 Id
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "DataAsset", DisplayName="资产数据Id")
	FPrimaryAssetId AssetId;
	
	// 武器类型
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category="Weapon", DisplayName="武器类型", meta=(AllowPrivateAccess))
	EWeaponType WeaponType;

	UPROPERTY(Replicated, BlueprintReadWrite, Category="Weapon", DisplayName="武器插槽名称", meta=(AllowPrivateAccess))
	FName EquippedWeaponSocketName;

	UPROPERTY(Replicated, BlueprintReadWrite, Category="Weapon", DisplayName="装备武器动画蒙太奇", meta=(AllowPrivateAccess))
	UAnimMontage* EquippedMontage;

	UPROPERTY(Replicated, BlueprintReadWrite, Category="Weapon", DisplayName="玩家装备武器后的状态", meta=(AllowPrivateAccess))
	EPlayerEquipState PlayerEquipState;
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

	// 武器类各自重载初始化函数

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category="Weapon", DisplayName="武器初始化")
	virtual void Init(ABlasterCharacter* InPlayerCharacter);

	UFUNCTION(BlueprintCallable, Category="Weapon", DisplayName="武器初始化处理")
	virtual void InitHandle(ABlasterCharacter* InPlayerCharacter);

	UFUNCTION(BlueprintCallable, Category="Weapon", DisplayName="武器装备")
	virtual void Equipment(bool Equipped);

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:

	FORCEINLINE ABlasterCharacter* GetPlayerCharacter() const { return PlayerCharacter; };
	
	FORCEINLINE USkeletalMeshComponent* GetWeaponMeshComponent() const {return WeaponMeshComponent;}

	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }

	FORCEINLINE EPlayerEquipState GetPlayerEquipState() const { return PlayerEquipState; };

	UFUNCTION(BlueprintCallable, Category="Weapon", DisplayName="获取装备武器骨骼插槽")
	FORCEINLINE FName GetEquippedWeaponSocket() const { return EquippedWeaponSocketName; };
};
