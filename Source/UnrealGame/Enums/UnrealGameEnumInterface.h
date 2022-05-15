// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

// 定义武器的枚举类型
UENUM(BlueprintType)
enum class ETurningInPlace : uint8
{
	ETIP_Left UMETA(DisplayName="Turning Left"),
	ETIP_Right UMETA(DisplayName="Turning Right"),
	ETIP_NotTuring UMETA(DisplayName="Not Turning"),
	
	ETIP_MAX UMETA(DisplayName="Default MAX")
};
