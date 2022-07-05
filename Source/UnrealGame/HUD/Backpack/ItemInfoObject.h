// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UObject/NoExportTypes.h"
#include "ItemInfoObject.generated.h"


UENUM(BlueprintType)
enum class EItemType : uint8
{
	Weapon UMETA(DisplayName="武器"),
	Consumables UMETA(DisplayName="消耗品"),
	Prop UMETA(DisplayName="道具")
};

UENUM()
enum class EMeshType : uint8
{
	SkeletalMesh UMETA(DisplayName="骨骼网格体"),
	StaticMesh UMETA(DisplayName="静态网格体")
};

USTRUCT(BlueprintType)
struct FItemInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="物品名称")
	FName ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="物品类型")
	EItemType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="网格类型")
	EMeshType MeshType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="骨骼网格体")
	USkeletalMesh* SkeletalMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="静态网格体")
	UStaticMesh* StaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="物品图像")
	UMaterialInterface* Image;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="物品所占用的格子长度，如 X = 2，Y = 1 代表在横向占用两格格子，纵向占用一格格子")
	FIntPoint OccupyCellXYLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="是否可堆叠")
	bool bIsCanStacking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="最大可堆叠数量")
	int MaxStackingNum;

	// UPROPERTY(DisplayName="是否可旋转，默认 OccupyCellXYLength 的 XY 相等的不可旋转")
	// int bIsCanRotate;

	// 在场景中的 Item 对应的 Actor 类
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="场景物品类")
	TSubclassOf<class AItemBase> ItemBaseClass;
};



USTRUCT(BlueprintType)
struct FBackpackItemInfo
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="物品数据表唯一Id")
	FString Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="背包物品唯一Id")
	FString BackpackId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="物品名称")
	FName ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="物品类型")
	EItemType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="物品图像")
	UMaterialInterface* Image;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="物品数量")
	int Num;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="物品所占用的格子长度，如 X = 2，Y = 1 代表在横向占用两格格子，纵向占用一格格子")
	FIntPoint OccupyCellXYLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="是否可堆叠")
	bool bIsCanStacking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="最大可堆叠数量")
	int MaxStackingNum;

	void Rotate()
	{
		FIntPoint Temp = OccupyCellXYLength;

		OccupyCellXYLength.X = Temp.Y;
		OccupyCellXYLength.Y = Temp.X;
	};

	void CreateDefaultData()
	{
		this->Id = nullptr;
		this->ItemName = TEXT("");
		this->ItemType = EItemType::Weapon;
		this->Image = nullptr;
		this->Num = 0;
		this->OccupyCellXYLength = FIntPoint(0,0);
		this->bIsCanStacking = false;
		this->MaxStackingNum = 0;
	}

	bool CanRotate(){ return !OccupyCellXYLength.X == OccupyCellXYLength.Y; }
};

struct FStruct_SetKeyFuncs:BaseKeyFuncs<FBackpackItemInfo, FString> {
private:
	typedef BaseKeyFuncs<FBackpackItemInfo,FString> Super;
public:
	typedef typename Super::KeyInitType KeyInitType;
	typedef typename Super::ElementInitType ElementInitType;
		
	static KeyInitType GetSetKey(ElementInitType Element) {
		return Element.BackpackId;
	}
		
	static bool Matches(KeyInitType A,KeyInitType B) {
		return A.Compare(B, ESearchCase::CaseSensitive)==0;
	}
		
	static uint32 GetKeyHash(KeyInitType Key) {
		return FCrc::StrCrc32(*Key);
	}
};



USTRUCT(BlueprintType)
struct FSceneItemInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="物品数据表唯一 Id")
	FString Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="物品数量")
	int Num;
};

/**
 * 
 */
UCLASS()
class UNREALGAME_API UItemInfoObject : public UObject
{
	GENERATED_BODY()


public:
	
	FName Id;

	bool bIsCanStacking;

	int Num;

	int MaxStackingNum;
	
	// 物品图像
	UMaterialInterface* Image;

	// 物品是否旋转
	bool bIsRotated;

	// 物品所占用的格子长度，如 X = 2，Y = 1 代表在横向占用两格格子，纵向占用一格格子
	FIntPoint OccupyCellLength;

	
	
};
