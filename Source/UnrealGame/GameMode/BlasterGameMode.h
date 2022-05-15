// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BlasterGameMode.generated.h"

/**
 * 
 */
UCLASS()
class UNREALGAME_API ABlasterGameMode : public AGameMode
{
	GENERATED_BODY()


public:

	// 被淘汰的玩家
	virtual void PlayerEliminated(class ABlasterCharacter* ElimmedCharacter
		, class ABlasterPlayerController* VictimController
		, ABlasterPlayerController* AttackerController);

	// 游戏模式 - 请求重生成
	virtual void RequestRespawn(class ACharacter* ElimmedCharacter, AController* ElimmedController);
	
protected:

	
	
private:

	
};
