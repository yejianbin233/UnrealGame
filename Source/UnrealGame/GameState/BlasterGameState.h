// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BlasterGameState.generated.h"

class ABlasterPlayerState;

/*
    * @name: ABlasterGameState - 冲击波玩家状态类
	* @description: 用于记录游戏内的状态，如：玩家最高得分等
	* 
	* @author: yejianbin
	* @createTime: 2022年05月17日 星期二 13:05:17
	*/
UCLASS()
class UNREALGAME_API ABlasterGameState : public AGameState
{
	GENERATED_BODY()
/*
 * Override Function
 */
public:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:

private:
	
/* Override End */

/*
* Custom Variable
*/
public:
	UPROPERTY(Replicated)
	TArray<ABlasterPlayerState*> TopScoringPlayers;

	
protected:

private:
	/* 最高得分 */
	float TopScore = 0.0f;
	
/* Custom Variable End*/


/*
 * Custom Method
 */
public:

	void UpdateToScore(ABlasterPlayerState* ScoringPlayer);
	
protected:

private:
	
/* Custom Method */

/*
 * Getter / Setter
 */

public:
	
/* Getter / Setter End */	
	
	
};
