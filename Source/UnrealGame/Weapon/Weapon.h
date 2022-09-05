// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnrealGame/Actor/ItemUse/ItemUse.h"
#include "UnrealGame/Enums/UnrealGameEnumInterface.h"
#include "UnrealGame/Enums/WeaponTypes.h"
#include "Weapon.generated.h"

UCLASS()
class UNREALGAME_API AWeapon : public AItemUse
{
	GENERATED_BODY()

	friend class ABlasterCharacter;
	friend class UCombatComponent;
	
protected:

	// 使用武器的角色可能不同，根据 Use() 函数来判断
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

	UPROPERTY(BlueprintReadWrite, Category="Weapon", DisplayName="武器初始化状态", meta=(AllowPrivateAccess))
	bool bIsInit;
public:	
	// Sets default values for this actor's properties
	AWeapon();

	// 武器类各自重载初始化函数
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category="Weapon", DisplayName="武器初始化")
	virtual void Init();

	// 武器初始化处理
	UFUNCTION(BlueprintCallable, Category="Weapon", DisplayName="武器初始化处理")
	virtual void InitHandle();

	// 武器使用(装备)
	virtual void Use(ABlasterCharacter* NewPlayerCharacter) override;

	UFUNCTION(Category="Use", DisplayName="装备", DisplayName="装备")
	virtual void Equip();

	// UFUNCTION(Server, Reliable, Category="Use", DisplayName="服务器作为客户端装备")
	// virtual void SNC_Equip();
	//
	UFUNCTION(Server, Reliable, Category="Use", DisplayName="服务器装备")
	virtual void SC_Equip();
	//
	UFUNCTION(Client, Reliable, Category="Use", DisplayName="客户端装备")
	virtual void CC_Equip();
	//
	UFUNCTION(NetMulticast, Reliable, Category="Use", DisplayName="多播装备")
	virtual void NM_Equip();
	//
	UFUNCTION(NetMulticast, Reliable, Category="Use", DisplayName="多播装备(除自身客户端)")
	virtual void NM_EquipExceptClient();

	UFUNCTION(Category="Use", DisplayName="取消装备", DisplayName="取消装备")
	virtual void UnEquip();

	UFUNCTION(NetMulticast, Reliable, Category="Equip", DisplayName="多播设置装备角色")
	virtual void SetEquptCharacter(ABlasterCharacter* NewPlayerCharacter);

	// UFUNCTION(Category="Use", DisplayName="服务器作为客户端取消装备")
	// virtual void SNC_UnEquip();
	//
	// UFUNCTION(Server, Reliable, Category="Use", DisplayName="服务器取消装备")
	// virtual void SC_UnEquip();
	//
	// UFUNCTION(Client, Reliable, Category="Use", DisplayName="客户端取消装备")
	// virtual void CC_UnEquip();
	//
	// UFUNCTION(NetMulticast, Reliable, Category="Use", DisplayName="多播取消装备")
	// virtual void NM_UnEquip();
	//
	// UFUNCTION(NetMulticast, Reliable, Category="Use", DisplayName="多播取消装备(除自身客户端)")
	// virtual void NM_UnEquipExceptClient();

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
