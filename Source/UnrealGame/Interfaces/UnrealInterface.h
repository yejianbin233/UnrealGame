// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Enums/UnrealGameEnumInterface.h"
#include "UObject/Interface.h"
#include "UnrealInterface.generated.h"


UINTERFACE(MinimalAPI, Blueprintable)
class UInteractiveInterface : public UInterface
{
	GENERATED_BODY()
};

class UNREALGAME_API IInteractiveInterface
{    
	GENERATED_BODY()

public:
	/** 在此处添加接口函数声明 */
	virtual void Interactive();
};

UINTERFACE(MinimalAPI, Blueprintable)
class ULightAbilityInterface : public UInterface
{
	GENERATED_BODY()
};

class UNREALGAME_API ILightAbilityInterface
{    
	GENERATED_BODY()

public:
	/** 在此处添加接口函数声明 */
	// 灯光打开
	virtual void TurnOn();
	// 灯光关闭
	virtual void TurnOff();
	// 灯光闪烁
	virtual void Flicker(ELightFlickerType NewLightFlickerType);

	virtual void EndFlicker();
	// 改变灯光颜色
	virtual void ChangeLightColor();
};