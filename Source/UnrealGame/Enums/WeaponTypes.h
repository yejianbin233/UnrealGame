#pragma once
#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_AssaultRifle UMETA(DisplayName = "Assault Rifle"),
	EWT_MAX UMETA(DisplayName = "Default Max"),
	
};

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECS_Reloading UMETA(DisplayName = "Unoccupied"),
	ECS_MAX UMETA(DisplayName = "DefaultMax"),

};
