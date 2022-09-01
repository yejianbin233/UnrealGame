// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractiveLight.h"
#include "GameFramework/Actor.h"
#include "InteractiveLightEffecController.generated.h"

/*
 * AInteractiveLightEffecController - 可交互灯光效果控制器
 * 1. 灯光闪烁效果
 * 2. 灯光颜色效果 - TODO
 */
UCLASS()
class UNREALGAME_API AInteractiveLightEffecController : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Light Effec Control", DisplayName="可交互灯光效果控制器静态网格体组件")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Light", DisplayName="被控制灯")
	TArray<AInteractiveLight*> Lights;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Light Effect", DisplayName="灯光效果类型", meta=(AllowPrivateAccess))
	ELightEffectType LightEffectType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Light Effect", DisplayName="灯光闪烁效果类型", meta=(AllowPrivateAccess))
	ELightFlickerType LightFlickerType;
	
public:
	
	// Sets default values for this actor's properties
	AInteractiveLightEffecController();

	void ApplyLightEffect(ELightEffectType NewLightEffecType);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
#if WITH_EDITOR
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif

	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:	
	FORCEINLINE UStaticMeshComponent* GetMeshComponent() const { return MeshComponent; }
};
