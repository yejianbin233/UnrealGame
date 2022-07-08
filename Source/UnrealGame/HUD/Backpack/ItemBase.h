// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemInfoObject.h"
#include "GameFramework/Actor.h"
#include "ItemBase.generated.h"

UCLASS()
class UNREALGAME_API AItemBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemBase();

	UFUNCTION(BlueprintCallable)
	void InitMesh(FItemInfo ItemInfo);

	UFUNCTION(BlueprintImplementableEvent, Category="Item Info")
	FBackpackItemInfo GetBackpackItemInfo();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Pickup", DisplayName="可拾取组件")
	class UPickableComponent* PickableComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Pickup", DisplayName="物品骨骼网格体组件")
	class USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Pickup", DisplayName="物品静态网格体组件")
	class UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Info", DisplayName="物品信息")
	FSceneItemInfo SceneItemInfo;

	UPROPERTY(EditDefaultsOnly, Category="Component Class")
	TEnumAsByte<ECollisionChannel> CollisionObjectType;

	UPROPERTY(EditDefaultsOnly, Category="Component Class")
	FName CollisionProfileName;
	
};
