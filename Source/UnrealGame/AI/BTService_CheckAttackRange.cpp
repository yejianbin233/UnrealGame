// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_CheckAttackRange.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

void UBTService_CheckAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// TODO 检查 Ai Pawn 与 TargetActor 之间的距离

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();

	if (ensure(BlackboardComponent))
	{
		AActor* TargetActor = Cast<AActor>(BlackboardComponent->GetValueAsObject("TargetActor"));

		if (TargetActor)
		{
			AAIController* OwnerAIController = OwnerComp.GetAIOwner();

			if (ensure(OwnerAIController))
			{
				APawn* AIPawn = OwnerAIController->GetPawn();
				if (ensure(AIPawn))
				{
					float Distance = FVector::Distance(TargetActor->GetActorLocation(), AIPawn->GetActorLocation());

					bool bWithinRange = Distance < 2000.0f;

					// 对视点(眼睛)和 TargetActor 两点之间进行线性追踪
					// bool bHasLineOfSigh = OwnerAIController->LineOfSightTo(TargetActor);

					// 当 AI 与 TargetActor 在一定范围内，且可以无物体遮挡时为 True。
					// bool CheckAttackRangeResult = bWithinRange && bHasLineOfSigh;
					bool CheckAttackRangeResult = bWithinRange;

					BlackboardComponent->SetValueAsBool(AttackRangeKey.SelectedKeyName, CheckAttackRangeResult);
					
				}
			}
		}
	}
}
