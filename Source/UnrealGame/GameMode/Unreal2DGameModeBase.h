// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Unreal2DGameModeBase.generated.h"

class AUnreal2DCharacter;
/**
 * 
 */
UCLASS()
class UNREALGAME_API AUnreal2DGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, Category="Spawn Player", DisplayName="玩家生成位置")
	FVector SpawnPlayerLocation;

public:

	UFUNCTION(BlueprintCallable, Category="Spawn Player", DisplayName="请求生成玩家角色")
	void RequestSpawnPlayerCharacter(TSubclassOf<AUnreal2DCharacter> PlayerCharacterClass, APlayerController* PlayerController);
};
