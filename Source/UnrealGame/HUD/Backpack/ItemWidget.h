// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../Backpack/ItemInfoObject.h"
#include "Blueprint/UserWidget.h"
#include "ItemWidget.generated.h"

/**
 * 
 */
UCLASS()
class UNREALGAME_API UItemWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category="Component")
	class UBackpackComponent* BackpackComponent;

	UPROPERTY(BlueprintReadWrite, Category="Widget", meta=(BindWidget), DisplayName="物品尺寸框")
	class USizeBox* ItemSizeBox;
	
	UPROPERTY(BlueprintReadWrite, Category="Widget", meta=(BindWidget), DisplayName="物品图像")
	class UImage* ItemImage;

	UPROPERTY(BlueprintReadWrite, Category="Widget", meta=(BindWidget), DisplayName="物品数量")
	class UTextBlock* ItemNum;

	UPROPERTY(BlueprintReadWrite, Category="Widget", meta=(BindWidget), DisplayName="物品边界")
	class UBorder* ItemBorder;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Widget Size", DisplayName="操作框尺寸")
	FVector2D Size;


private:
	

public:

	UFUNCTION(BlueprintCallable, Category="Item Operator", DisplayName="显示物品操作控件")
	void ShowItemOperate(UItemInfoObject* ItemInfoObject, FVector2D Position);
	
protected:

private:
	
	
};
