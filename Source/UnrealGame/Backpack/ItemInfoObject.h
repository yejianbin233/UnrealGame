// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UnrealGame/Actor/ItemUse/ItemUse.h"
#include "UObject/NoExportTypes.h"
#include "ItemInfoObject.generated.h"


UENUM(BlueprintType)
enum class EItemType : uint8
{
	Weapon UMETA(DisplayName="武器"),
	Consumables UMETA(DisplayName="消耗品"),
	Projectile UMETA(DisplayName="子弹"),
	Prop UMETA(DisplayName="道具")
};

UENUM()
enum class EMeshType : uint8
{
	SkeletalMesh UMETA(DisplayName="骨骼网格体"),
	StaticMesh UMETA(DisplayName="静态网格体")
};

/*
 * 物品的数据结构，用于物品数据表
 */
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

	// 在场景中的 Item 对应的 Actor 类
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="场景物品类")
	TSubclassOf<class AItemBase> ItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="物品使用类")
	TSubclassOf<class AItemUse> ItemUseClass;
	
};

/*
 * 在背包中保存的物品数据
 */
USTRUCT(BlueprintType)
struct FBackpackItemInfo
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, DisplayName="物品数据表唯一Id")
	FString Id;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, DisplayName="物品名称")
	FName ItemName;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, DisplayName="物品类型")
	EItemType ItemType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, DisplayName="物品图像")
	UMaterialInterface* Image;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="物品数量")
	int Num;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, DisplayName="物品所占用的格子长度，如 X = 2，Y = 1 代表在横向占用两格格子，纵向占用一格格子")
	FIntPoint OccupyCellXYLength;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, DisplayName="是否可堆叠")
	bool bIsCanStacking;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, DisplayName="是否背包唯一类型物品")
	bool bIsBackpackUnique;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, DisplayName="最大可堆叠数量")
	int MaxStackingNum;

	UPROPERTY(BlueprintReadWrite, DisplayName="是否已旋转")
	bool bIsRotated;

	// TODO 冗余数据 - 背包数据暂时未设想太多，暂时将所有数据集中在一起
	UPROPERTY(BlueprintReadWrite, DisplayName="远程武器装备子弹数量")
	float LongRangeWeaponEquipAmmoNum;

	FBackpackItemInfo(): ItemType(), Image(nullptr), Num(0), bIsCanStacking(false), bIsBackpackUnique(false),
	                     MaxStackingNum(0),
	                     bIsRotated(false),
	                     LongRangeWeaponEquipAmmoNum(0)
	{
	};
	
	void SetDefaultData()
	{
		this->Id = nullptr;
		this->ItemName = TEXT("");
		this->ItemType = EItemType::Weapon;
		this->Image = nullptr;
		this->Num = 0;
		this->OccupyCellXYLength = FIntPoint(0,0);
		this->bIsCanStacking = false;
		this->MaxStackingNum = 0;
		this->LongRangeWeaponEquipAmmoNum = 0;
	}

	void Rotate()
	{
		this->bIsRotated = !this->bIsRotated;

		int X = OccupyCellXYLength.X;
		OccupyCellXYLength.X = OccupyCellXYLength.Y;
		OccupyCellXYLength.Y = X;
	};

	bool CanRotate(){ return OccupyCellXYLength.X != OccupyCellXYLength.Y; }
};

USTRUCT(BlueprintType)
struct FBackpackInfoOverride : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, DisplayName="场景物品Actor")
	AItemBase* Item;

	FBackpackItemInfo BackpackItemInfo;
};

// /*
//  * 场景中的物品所具有的结构，用于存储物品部分数组，可通过数据表获取更多的物品数据
//  */
// USTRUCT(BlueprintType)
// struct FSceneItemInfo
// {
// 	GENERATED_BODY()
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="物品数据表唯一 Id")
// 	FString Id;
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="物品数量")
// 	int Num;
// };

/**
 * 背包物品数据对象
 */
UCLASS(Blueprintable, BlueprintType)
class UNREALGAME_API UItemInfoObject : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, Category="Item Data", DisplayName="物品数据")
	FBackpackItemInfo BackpackItemInfo;

	UPROPERTY(BlueprintReadWrite, Category="Scene Item", DisplayName="场景物品")
	AItemBase* SceneItem;

	UPROPERTY(BlueprintReadWrite, Category="Use Item", DisplayName="使用物品")
	AItemUse* UseItem;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, DisplayName="背包物品唯一Id")
	FString BackpackId;

	// UPROPERTY(VisibleAnywhere, BlueprintReadWrite, DisplayName="加入服务器背包唯一ID")
	// FString ServerPushId;

public:

	void Init(AItemBase* Item);

	virtual bool IsSupportedForNetworking () const override { return true; };

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintCallable, DisplayName="旋转")
	void Rotate()
	{
		this->BackpackItemInfo.bIsRotated = !this->BackpackItemInfo.bIsRotated;
		const int X = BackpackItemInfo.OccupyCellXYLength.X;
		BackpackItemInfo.OccupyCellXYLength.X = BackpackItemInfo.OccupyCellXYLength.Y;
		BackpackItemInfo.OccupyCellXYLength.Y = X;
	};

	UFUNCTION(BlueprintCallable, DisplayName="是否可旋转")
	bool CanRotate()
	{
		const bool Result = BackpackItemInfo.OccupyCellXYLength.X != BackpackItemInfo.OccupyCellXYLength.Y;
		return Result;
	}

};

/*
 * 用于物品 Set 检索数据
 */
struct FStruct_SetKeyFuncs:BaseKeyFuncs<UItemInfoObject, FString> {
private:
	typedef BaseKeyFuncs<UItemInfoObject*, FString> Super;
public:
	typedef typename Super::KeyInitType KeyInitType;
	typedef typename Super::ElementInitType ElementInitType;
		
	static KeyInitType GetSetKey(ElementInitType Element) {
		return Element->BackpackId;
	}
		
	static bool Matches(KeyInitType A,KeyInitType B) {
		return A.Compare(B, ESearchCase::CaseSensitive)==0;
	}
		
	static uint32 GetKeyHash(KeyInitType Key) {
		return FCrc::StrCrc32(*Key);
	}
};
