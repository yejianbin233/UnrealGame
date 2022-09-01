// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "UnrealGame/Interfaces/UnrealInterface.h"
#include "InteractiveLight.generated.h"

DECLARE_EVENT_OneParam(AFlashLight, FOnLightFlickerStared, ELightFlickerType /*灯光闪烁类型*/)

DECLARE_EVENT(AFlashLight, FOnLightFlickerEnded)

/*
 * AInteractiveLight - 可交互的灯光 Actor
 *
 * 1. 打开 / 关闭功能
 * 2. 灯光闪烁
 * 3. .. TODO
 */
UCLASS(NotBlueprintable)
class UNREALGAME_API AInteractiveLight : public AActor, public IInteractiveInterface, public ILightAbilityInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Mesh", DisplayName="手电筒网格体组件")
	class UStaticMeshComponent* MeshComponent;

	// 周期闪烁时间轴
	FTimeline PeroidicFlickerTimeline;

	FOnTimelineFloat OnPeroidicFlickerStarted;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Light Flicker", DisplayName="周期闪烁曲线资产")
	UCurveFloat* PeriodicFlickerCurve;

	FName FlickerValueName = TEXT("FlickerValue");

	// 临时闪烁时间轴
	FTimeline TemporaryFlickerTimeline;

	FOnTimelineFloat OnTemporaryFlickerStarted;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Light Flicker", DisplayName="临时闪烁曲线资产")
	UCurveFloat* TemporaryFlickerCurve;

	FOnLightFlickerStared OnLightFlickerStared;
	
	FOnLightFlickerEnded OnLightFlickerEnded;
	
protected:
	UPROPERTY(BlueprintReadWrite, Category="Light State", DisplayName="灯光打开/关闭状态", meta=(AllowPrivateAccess))
	ELightState LightState;

	UPROPERTY(BlueprintReadWrite, Category="Light Effect State", DisplayName="灯光特殊效果状态", meta=(AllowPrivateAccess))
	ELightEffectType LightEffectType;

	UPROPERTY(BlueprintReadWrite, Category="Light", DisplayName="灯光默认强度", meta=(AllowPrivateAccess))
	float DefaultLightIntensity = 5000.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Interactive", DisplayName="交互状态", meta=(AllowPrivateAccess))
	EInteractiveState InteractiveState;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Light Flicker", DisplayName="灯光闪烁类型", meta=(AllowPrivateAccess))
	ELightFlickerType LightFlickerType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Light Flicker", DisplayName="是否循环闪烁", meta=(AllowPrivateAccess))
	bool bIsFlickerLooping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Light Material", DisplayName="手电筒灯光函数材质实例", meta=(AllowPrivateAccess))
	UMaterialInstance* FlashLightFunctionMaterialInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Light Material", DisplayName="手电筒灯光函数动态材质实例", meta=(AllowPrivateAccess))
	UMaterialInstanceDynamic* FlashLightFunctionMaterialInstanceDynamic;
	
public:	
	// Sets default values for this actor's properties
	AInteractiveLight();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category="Flash Light", DisplayName="手电筒交互")
	virtual void Interactive() override;

private:
	UFUNCTION(BlueprintCallable, Category="Flicker", DisplayName="灯光周期闪烁处理")
	void PeriodicFlickerHandle(float Value);

	UFUNCTION(BlueprintCallable, Category="Flicker", DisplayName="灯光临时闪烁处理")
	void TemporaryFlickerHandle(float Value);

	UFUNCTION(BlueprintCallable, Category="Flicker", DisplayName="灯光开始闪烁")
	void StartLightFlicker(ELightFlickerType InLightFlickerType);

	UFUNCTION(BlueprintCallable, Category="Flicker", DisplayName="灯光结束闪烁")
	void EndLightFlicker();

public:
	
	FORCEINLINE UStaticMeshComponent* GetMesh() const { return MeshComponent; }
};
