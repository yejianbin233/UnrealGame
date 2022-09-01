// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Engine/EngineBaseTypes.h"
#include "LightAbilityComponent.generated.h"

// TODO - 等灯光相关功能继续拓展时考虑是否将其抽象成组件
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALGAME_API ULightAbilityComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULightAbilityComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category="Light Interactive", DisplayName="手电筒交互")
	virtual void Interactive();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
