// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

UENUM(BlueprintType)
enum class EPlayerEquipState : uint8
{
	UnArmed UMETA(DisplayName="未装备"),
	Rifle UMETA(DisplayName="步枪"),
	Pistol UMETA(DisplayName="手枪"),
	Sword UMETA(DisplayName="剑"),
};

UENUM(BlueprintType)
enum class EPickableObjectState : uint8
{
	Default UMETA(DisplayName="默认"),
	Pickup UMETA(DisplayName="拾取"),
	Equip UMETA(DisplayName="装备"),
};

UENUM(BlueprintType)
enum class EWeaponPlay : uint8
{
	Fire UMETA(DisplayName="开火"),
	Reload UMETA(DisplayName="装填子弹"),
	Dry UMETA(DisplayName="缺少子弹"),
};

UENUM(BlueprintType)
enum class EPickupResult : uint8
{
	All UMETA(DisplayName="拾取全部"),
	Part UMETA(DisplayName="拾取部分"),
	Fail UMETA(DisplayName="失败"),
};

UENUM(BlueprintType)
enum class EReapperMode : uint8
{
	// 临时重现，延迟后销毁
	Temporary UMETA(DisplayName="临时"),

	// 序列重现
	Sequence UMETA(DisplayName="序列"),
};

UENUM(BlueprintType)
enum class EProjectileHitMode : uint8
{
	// 预测 - 在服务器的生成的子弹不采用实时碰撞检测处理，而实现预测功能检测子弹伤害碰撞
	Predict UMETA(DisplayName="预测"),

	// 实时 - 在服务器的生成的子弹采用实时碰撞检测处理
	RealTime UMETA(DisplayName="实时"),
};