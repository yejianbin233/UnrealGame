// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UnrealGame/Enums/WeaponTypes.h"
#include "Weapon.generated.h"

UCLASS()
class UNREALGAME_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

	// 武器类各自重载初始化函数
	UFUNCTION(BlueprintCallable, Category="Weapon", DisplayName="初始化")
	virtual void Init();

protected:

	UPROPERTY(BlueprintReadOnly, Category="Reference", DisplayName="玩家角色", meta=(AllowPrivateAccess))
	class ABlasterCharacter* BlasterOwnerCharacter;

	UPROPERTY(BlueprintReadOnly, Category="Reference", DisplayName="玩家控制器", meta=(AllowPrivateAccess))
	class ABlasterPlayerController* BlasterPlayerOwnerController;
	
	// 枪骨骼网格体
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Mesh", DisplayName="武器网格体", meta=(AllowPrivateAccess))
	class USkeletalMeshComponent* WeaponMeshComponent;

	// 武器类型
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon", DisplayName="武器类型", meta=(AllowPrivateAccess))
	EWeaponType WeaponType;
	
public:
	FORCEINLINE USkeletalMeshComponent* GetWeaponMeshComponent() const {return WeaponMeshComponent;}

	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
};
