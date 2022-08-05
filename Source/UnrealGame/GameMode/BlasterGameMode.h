// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BlasterGameMode.generated.h"


/*
 * 自定义游戏模式比赛状态(MatchState)
 */
namespace MatchState
{
	extern UNREALGAME_API const FName Cooldown;
}

/**
 * 
 */
UCLASS()
class UNREALGAME_API ABlasterGameMode : public AGameMode
{
	GENERATED_BODY()


public:

	ABlasterGameMode();

	virtual void BeginDestroy() override;

	virtual void Tick(float DeltaSeconds) override;

	// 被淘汰的玩家
	virtual void PlayerEliminated(class ABlasterCharacter* ElimmedCharacter
		, class ABlasterPlayerController* VictimController
		, ABlasterPlayerController* AttackerController);

	// 游戏模式 - 请求重生成
	virtual void RequestRespawn(class ACharacter* ElimmedCharacter, AController* ElimmedController);

	UFUNCTION(BlueprintImplementableEvent, Category="Data Asset Manager", DisplayName="加载主资产")
	void LoadPrimaryDataAsset();

	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 10.0f;

	UPROPERTY(EditDefaultsOnly)
	float MatchTime = 120.0f;

	UPROPERTY(EditDefaultsOnly)
	float CooldownTime = 10.0f;

	float LevelStartingTime = 0.0f;
	
protected:

	virtual void BeginPlay() override;

	virtual void OnMatchStateSet() override;
	
private:

	float CountdownTime = 0.0f;


public:

	FORCEINLINE float GetCountdownTime() const { return CountdownTime; }
		
};
