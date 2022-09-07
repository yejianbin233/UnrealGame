// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/VerticalBox.h"
#include "UnrealGame/Enums/UnrealGameEnumInterface.h"
#include "ItemOperatorWidget.generated.h"

class UItemInfoObject;
class UItemOptionsWidget;

USTRUCT(BlueprintType)
struct FItemOptions
{
	GENERATED_BODY()

	EItemOptionsType OptionType;
	FString OptionName;
};
/**
 * 
 */
UCLASS()
class UNREALGAME_API UItemOperatorWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, Category="Component")
	class UBackpackComponent* BackpackComponent;

	UPROPERTY(BlueprintReadWrite, Category="Item Options", meta=(BindWidget), DisplayName="物品选项框")
	UVerticalBox* ItemOptionsBox;

	UPROPERTY(BlueprintReadWrite, Category="Item Info", DisplayName="物品信息对象")
	UItemInfoObject* ItemInfoObject;

private:

	UPROPERTY(BlueprintReadWrite, Category="Item Render", DisplayName="是否展示物品", meta=(AllowPrivateAccess))
	bool bIsShowItenRender=false;
	
public:

	UFUNCTION(BlueprintCallable, Category="Item Operator", DisplayName="重置操作")
	virtual void Reset(UItemInfoObject* NewItemInfoObject, TArray<FItemOptions>& Optionses);

	UFUNCTION(BlueprintImplementableEvent, Category="Item Options", DisplayName="创建物品选项控件")
	UItemOptionsWidget* CreateItemOptionWidget();

	UFUNCTION()
	void Use();

	UFUNCTION()
	void Equip();

	UFUNCTION()
	void Show();

	UFUNCTION(BlueprintCallable, Category="Item Render", DisplayName="取消显示物品")
	void UnShow();

	UFUNCTION()
	void Throw();

	UFUNCTION()
	void Cancel();
};
