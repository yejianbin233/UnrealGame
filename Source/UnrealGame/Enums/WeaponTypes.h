#pragma once
#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	LongRangeWeapon UMETA(DisplayName = "远程武器"),
	MeleeWeapon UMETA(DisplayName = "近战武器"),
	ThrowableWeapon UMETA(DisplayName = "可投掷武器"),
};

UENUM(BlueprintType)
enum class ELongRangeWeaponType : uint8
{
	Pistol UMETA(DisplayName = "Pistol"),
	Rifle UMETA(DisplayName = "Rifle"),
	Shotgun UMETA(DisplayName = "Shotgun"),
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
	Idle UMETA(DisplayName = "Idle"),
	Combat UMETA(DisplayName = "Combat"),
	ECS_Reloading UMETA(DisplayName = "Reloading"),
};