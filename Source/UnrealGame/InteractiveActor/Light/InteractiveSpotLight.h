// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractiveLight.h"
#include "UnrealGame/Enums/UnrealGameEnumInterface.h"
#include "InteractiveSpotLight.generated.h"



/*
 * AInteractiveSpotLight - 可交互点光源：
 * 1. 手电筒
 * 2. 台灯
 * 3. 盏灯
 * 4. ...
 */
UCLASS(Blueprintable)
class UNREALGAME_API AInteractiveSpotLight : public AInteractiveLight
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Mesh", DisplayName="聚光灯光源组件")
	class USpotLightComponent* SpotLightComponent;

public:

	// Sets default values for this actor's properties
	AInteractiveSpotLight();
	
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
	FORCEINLINE USpotLightComponent* GetSpotLightComponent() const { return SpotLightComponent; }
};


