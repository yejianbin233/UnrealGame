// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemInfoObject.h"
#include "Components/ActorComponent.h"
#include "UnrealGame/Character/BlasterCharacter.h"
#include "BackpackComponent.generated.h"

USTRUCT(BlueprintType)
struct FPositionItem
{
	GENERATED_BODY()

	FVector2D Position;
	FBackpackItemInfo* Item;
};

DECLARE_EVENT(UBackpackComponent, FOnBackpackItemChanged)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType )
class UNREALGAME_API UBackpackComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBackpackComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool TryAddItem(FBackpackItemInfo* Item);
	
	bool IsHadPlace(FBackpackItemInfo* Item, int* Index);

	bool PlaceCheck(FBackpackItemInfo* Item, int* Index);

	void CoordinateConvert(int* Index, FVector2D* Position, bool* bToIndex = nullptr);

	void GetAllItem(TArray<FPositionItem>* PositionItems);
	
	void GetItems(TArray<FPositionItem>* PositionItems, FString Id);

	bool IsValidPosition(FVector2D Position);

	UFUNCTION(BlueprintCallable, Category="BackpackWidget", DisplayName="打开或关闭背包控件")
	void OpenOrCloseBackpack();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	void AddNewItem(FBackpackItemInfo* Item, int Index);

public:

	UPROPERTY()
	class APlayerController* PlayerController;

	UPROPERTY()
	class ABlasterCharacter* BlasterCharacter;

	UPROPERTY(BlueprintReadOnly, Category="Widget", DisplayName="背包控件")
	class UBackpackWidget* BackpackWidget;

	UPROPERTY(EditAnywhere, Category="Widget", DisplayName="背包控件类")
	TSubclassOf<class UUserWidget> BackpackWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Backpack Setting", DisplayName="背包行数")
	int Row;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Backpack Setting", DisplayName="背包列数")
	int Column;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Backpack Setting", DisplayName="背包单元格大小")
	float CellSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Backpack Setting", DisplayName="背包物品")
	TArray<FBackpackItemInfo> Items;

	FOnBackpackItemChanged OnBackpackItemChanged;

	
		
};

