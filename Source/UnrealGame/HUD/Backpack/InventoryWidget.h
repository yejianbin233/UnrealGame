// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Engine/DataTable.h"
#include "InventoryWidget.generated.h"


USTRUCT(BlueprintType)
struct FLink : public FTableRowBase 
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="Link")
	FVector2D Start;
	UPROPERTY(BlueprintReadWrite, Category="Link")
	FVector2D End;
	
};
/**
 * 
 */
UCLASS()
class UNREALGAME_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()


public:

	UPROPERTY(BlueprintReadWrite, Category="Component")
	class UBackpackComponent* BackpackComponent;

	UPROPERTY(BlueprintReadWrite, Category="Widget", DisplayName="背包边界框", meta=(BindWidget))
	UCanvasPanel* BackpackBorder;

	UPROPERTY(BlueprintReadWrite, Category="Widget", DisplayName="背包内容上下文框", meta=(BindWidget))
	UCanvasPanel* BackpackContent;

	UPROPERTY(BlueprintReadWrite, Category="Widget", DisplayName="背包背景", meta=(BindWidget))
	UBorder* BackpackBackground;

	UPROPERTY(BlueprintReadWrite, Category="Properties")
	TArray<FLink> Links;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Properties")
	TSubclassOf<UUserWidget> InventoryUIClass;

	UPROPERTY(BlueprintReadWrite, Category="Properties")
	float CellSize;

	UFUNCTION(BlueprintCallable, Category="Backpack Function")
	void Init(UBackpackComponent* InBackpackComponent);

	UFUNCTION(BlueprintCallable, Category="Backpack Function")
	void SetBackpackBorderSize();

	UFUNCTION(BlueprintCallable, Category="Backpack Function")
	void InitLinks();

	UFUNCTION(BlueprintCallable, Category="Backpack Function")
	void Refresh();
};
