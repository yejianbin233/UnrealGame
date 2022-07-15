#pragma once
#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_AssaultRifle UMETA(DisplayName = "Gun"),
	EWT_RocketLauncher UMETA(DisplayName = "Melee"),
	EWT_Pistol UMETA(DisplayName = "Missile"),
};

UENUM(BlueprintType)
enum class EGunWeaponType : uint8
{
	Pistol UMETA(DisplayName = "Pistol"),
	Rifle UMETA(DisplayName = "Rifle"),
	Shotgun UMETA(DisplayName = "Rifle"),
	RocketLauncher UMETA(DisplayName = "Rocket Launcher"),
};

UENUM(BlueprintType)
enum class EMeleeWeaponType : uint8
{
	AssaultRifle UMETA(DisplayName = "Sword"),
	RocketLauncher UMETA(DisplayName = "Knife"),
};

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECS_Reloading UMETA(DisplayName = "Unoccupied"),
	ECS_MAX UMETA(DisplayName = "DefaultMax"),

};
