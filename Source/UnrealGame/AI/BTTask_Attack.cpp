// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack.h"

#include "AIController.h"
#include "GameFramework/Character.h"

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* MyAiController = OwnerComp.GetAIOwner();

	if (ensure(MyAiController))
	{
		ACharacter* MyPawn = Cast<ACharacter>(MyAiController->GetPawn());

		if (MyPawn == nullptr)
		{
			return EBTNodeResult::Type::Failed;
		}
	}
	
	return EBTNodeResult::Type::Succeeded;
}
