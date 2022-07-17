// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleRandomMovementProcessor.h"

#include "MassCommonTypes.h"
#include "MassCommonFragments.h"
#include "SimpleRandomMovementTrait.h"

USimpleRandomMovementProcessor::USimpleRandomMovementProcessor()
{
	// 自动注册，设置了自动注册后，处理器不需要其他设置即可自动生效，前提是具有处理器所需要的片段（通过在 Mass Entity Config Asset 中添加"特征Trait"）。
	bAutoRegisterWithProcessingPhases = true;

	ExecutionFlags = (int32)EProcessorExecutionFlags::All;

	// 设置为在回避处理分组之前执行
	ExecutionOrder.ExecuteBefore.Add(UE::Mass::ProcessorGroupNames::Avoidance);
}

void USimpleRandomMovementProcessor::ConfigureQueries()
{
	Super::ConfigureQueries();

	// 1. 添加片段需求以及如何访问数据(读写...)
	// FTransformFragment 需要 #include "MassCommonFragments.h"
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FSimpleMovementFragment>(EMassFragmentAccess::ReadWrite);
}

void USimpleRandomMovementProcessor::Execute(UMassEntitySubsystem& EntitySubsystem, FMassExecutionContext& Context)
{
	// Super::Execute(EntitySubsystem, Context);

	// 2. 使用 Execute 方法，遍历 "EntityQuery" 当前数据块中的所有实体，定义逻辑处理片段数据
	
	EntityQuery.ForEachEntityChunk(EntitySubsystem, Context, ([this](FMassExecutionContext& Context)
	{
		const TArrayView<FTransformFragment> TransformsList = Context.GetMutableFragmentView<FTransformFragment>();

		const TArrayView<FSimpleMovementFragment> SimpleMovementsList = Context.GetMutableFragmentView<FSimpleMovementFragment>();

		const float WorldDeltaTime = Context.GetDeltaTimeSeconds();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); ++EntityIndex)
		{
			FTransform& Transform = TransformsList[EntityIndex].GetMutableTransform();
			FVector& MoveTarget = SimpleMovementsList[EntityIndex].Target;

			FVector CurrentLocation = Transform.GetLocation();
			FVector TargetVector = MoveTarget - CurrentLocation;

			if (TargetVector.Size() <= 20.f)
			{
				MoveTarget = FVector(FMath::RandRange(-1.f, 1.f) * 1000.f
					, FMath::RandRange(-1.f, 1.f) * 1000.f, CurrentLocation.Z);
			}
			else
			{
				Transform.SetLocation(CurrentLocation + TargetVector.GetSafeNormal() * 400.f * WorldDeltaTime);
			}
		}
	}));
}
