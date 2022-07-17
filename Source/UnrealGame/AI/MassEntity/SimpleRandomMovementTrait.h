// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTraitBase.h"
#include "MassEntityTypes.h"
#include "SimpleRandomMovementTrait.generated.h"

USTRUCT()
struct FSimpleMovementFragment : public FMassFragment
{
	GENERATED_BODY()

	FVector Target;
};

/**
 * 
 */
UCLASS()
class UNREALGAME_API USimpleRandomMovementTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

protected:
	// 将项目附加到"特征"所需的实体模板中。
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, UWorld& World) const override;
};
