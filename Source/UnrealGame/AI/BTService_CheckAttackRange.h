// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_CheckAttackRange.generated.h"

/**
 * 
 */
UCLASS()
class UNREALGAME_API UBTService_CheckAttackRange : public UBTService
{
	GENERATED_BODY()


protected:

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	// 定义行为树"键"选择器，便于在编辑器中调整
	UPROPERTY(EditAnywhere, Category = "AI", meta=(AllowPrivateAccess))
	FBlackboardKeySelector AttackRangeKey;

	
};
