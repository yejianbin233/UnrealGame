// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UnrealGame/Backpack/ItemRender.h"
#include "ItemRenderWidget.generated.h"

/**
 * 
 */
UCLASS()
class UNREALGAME_API UItemRenderWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category="Item Render", DisplayName="物品渲染Actor")
	AItemRender* ItemRender;

public:
	
	UFUNCTION(BlueprintCallable, Category="Item Render Input", DisplayName="开启物品渲染输入")
	void EnableItemRenderInput();

	UFUNCTION(BlueprintCallable, Category="Item Render Input", DisplayName="关闭物品渲染输入")
	void DisableItemRenderInput();
};
