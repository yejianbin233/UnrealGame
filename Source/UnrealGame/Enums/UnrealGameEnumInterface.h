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
	AddNewItem_All UMETA(DisplayName="添加新物品-拾取全部"),
	AddNewItem_Part UMETA(DisplayName="添加新物品-拾取部分"),

	StackAdd_All UMETA(DisplayName="叠加拾取全部"),
	StackAdd_Part UMETA(DisplayName="叠加拾取部分"),

	Fail UMETA(DisplayName="失败"),
	BackpackUnique_Fail UMETA(DisplayName="失败，背包物品唯一，不可添加"),
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

UENUM(BlueprintType)
enum class ECameraViewPointMode : uint8
{
	// 第一人称视点
	FirstPerson UMETA(DisplayName="第一人称视点"),

	// 第三人称视点
	ThreePerson UMETA(DisplayName="第三人称视点"),

	Debug_RightPoint UMETA(DisplayName="调试模式右视点"),

	Debug_TopPoint UMETA(DisplayName="调试模式顶部视点"),
};

UENUM(BlueprintType)
enum class ECameraShotMode : uint8
{
	// 标准截图
	Shot UMETA(DisplayName="标准截图"),

	// 高清截图
	HighResShot UMETA(DisplayName="高清截图"),

	// 自定义截图
	CustomShot UMETA(DisplayName="自定义截图"),
};

UENUM(BlueprintType)
enum class EPlayerInputMappingContent : uint8
{
	// 标准截图
	Shooter UMETA(DisplayName="射手模式输入上下文"),

	// 高清截图
	Camera UMETA(DisplayName="摄像机模式输入上下文"),
};

UENUM(BlueprintType)
enum class ELightState : uint8
{
	// 打开状态
	TurnOn UMETA(DisplayName="打开状态"),

	// 关闭状态
	TurnOff UMETA(DisplayName="关闭状态"),
};

UENUM(BlueprintType)
enum class ELightEffectType : uint8
{
	// 正常状态
	Normal UMETA(DisplayName="正常状态"),
	
	// 打开状态
	Flicker UMETA(DisplayName="闪烁状态"),
};

UENUM(BlueprintType)
enum class EInteractiveState : uint8
{
	// 可交互状态
	Interactable UMETA(DisplayName="可交互"),
	
	// 不可交互状态
	Not_Interactacle UMETA(DisplayName="不可交互"),
};

UENUM(BlueprintType)
enum class ELightFlickerType : uint8
{
	// 临时闪烁
	Temporary UMETA(DisplayName="临时闪烁"),
	
	// 周期闪烁
	Periodic UMETA(DisplayName="周期闪烁"),
};

UENUM(BlueprintType)
enum class EItemOptionsType : uint8
{
	Equip UMETA(DisplayName="装备"),
	Use UMETA(DisplayName="使用"),
	Show UMETA(DisplayName="展示"),
	Throw UMETA(DisplayName="丢弃"),
	Cancle UMETA(DisplayName="取消"),
};