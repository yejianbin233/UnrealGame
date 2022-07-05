// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemInfoObject.h"
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

	UPROPERTY(BlueprintReadWrite, Category="Widget", meta=(BindWidget), DisplayName="物品尺寸框")
	class USizeBox* ItemSizeBox;
	
	UPROPERTY(BlueprintReadWrite, Category="Widget", meta=(BindWidget), DisplayName="物品图像")
	class UImage* ItemImage;

	UPROPERTY(BlueprintReadWrite, Category="Widget", meta=(BindWidget), DisplayName="物品数量")
	class UTextBlock* ItemNum;

	UPROPERTY(BlueprintReadWrite, Category="Propertie")
	FVector2D Position;

	UPROPERTY(BlueprintReadWrite, Category="Propertie")
	FBackpackItemInfo ItemInfo;

	UFUNCTION(BlueprintImplementableEvent)
	void Init(FBackpackItemInfo InItemInfo, FVector2D InPosition);
	
protected:
	
	
private:
	
	
};
