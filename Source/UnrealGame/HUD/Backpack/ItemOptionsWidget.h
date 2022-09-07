// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "ItemOptionsWidget.generated.h"

struct FItemOptions;
/**
 * 
 */
UCLASS()
class UNREALGAME_API UItemOptionsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite, Category="Option", DisplayName="选项按钮", meta=(BindWidget))
	UButton* OptionButton;
	
	UPROPERTY(BlueprintReadWrite, Category="Option", DisplayName="选项文本", meta=(BindWidget))
	UTextBlock* OptionText;

public:

	virtual void Init(FItemOptions Options);
};
