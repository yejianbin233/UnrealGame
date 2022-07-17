// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "SimpleRandomMovementProcessor.generated.h"

/**
 * 
 */
UCLASS()
class UNREALGAME_API USimpleRandomMovementProcessor : public UMassProcessor
{
	GENERATED_BODY()

public:
	USimpleRandomMovementProcessor();

protected:

	// 配置数据，定义如何处理实体
	virtual void ConfigureQueries() override;
	// 处理器处理逻辑
	virtual void Execute(UMassEntitySubsystem& EntitySubsystem, FMassExecutionContext& Context) override;

private:
	FMassEntityQuery EntityQuery;
};
