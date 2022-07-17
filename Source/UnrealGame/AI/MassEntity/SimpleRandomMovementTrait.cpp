// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleRandomMovementTrait.h"
#include "MassEntityTemplateRegistry.h"

void USimpleRandomMovementTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, UWorld& World) const
{
	Super::BuildTemplate(BuildContext, World);

	// 将片段添加到实体，指定片段类型
	BuildContext.AddFragment<FSimpleMovementFragment>();
}
