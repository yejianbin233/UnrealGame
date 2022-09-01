// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractiveLight.h"
#include "InteractivePointLight.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class UNREALGAME_API AInteractivePointLight : public AInteractiveLight
{
	GENERATED_BODY()
		
public:	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Mesh", DisplayName="点光源组件")
	class UPointLightComponent* PointLightComponent;

public:

	// Sets default values for this actor's properties
	AInteractivePointLight();
	
	UFUNCTION(BlueprintCallable, Category="Flash Light", DisplayName="打开")
	virtual void TurnOn() override;
	
	UFUNCTION(BlueprintCallable, Category="Flash Light", DisplayName="关闭")
	virtual void TurnOff() override;

	UFUNCTION(BlueprintCallable, Category="Flash Light", DisplayName="灯光闪烁")
	virtual void Flicker(ELightFlickerType NewLightFlickerType) override;

	UFUNCTION(BlueprintCallable, Category="Flash Light", DisplayName="结束灯光闪烁")
	virtual void EndFlicker() override;

	UFUNCTION(BlueprintCallable, Category="Flash Light", DisplayName="改变灯光颜色")
	virtual void ChangeLightColor() override;

protected:

	virtual void OnConstruction(const FTransform& Transform) override;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	FORCEINLINE UPointLightComponent* GetPointLightComponent() const { return PointLightComponent; }
};
