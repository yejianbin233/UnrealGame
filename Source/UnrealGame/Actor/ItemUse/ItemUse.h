// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemUse.generated.h"

/*
 * AItemAbility - 物品在使用时所提供的能力。
 * 
 */
UCLASS()
class UNREALGAME_API AItemUse : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemUse();

	UFUNCTION(BlueprintCallable, Category="Item", DisplayName="物品使用")
	virtual void Use(class ABlasterCharacter* PlayerCharacter);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
