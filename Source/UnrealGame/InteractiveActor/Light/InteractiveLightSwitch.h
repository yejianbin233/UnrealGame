// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractiveLight.h"
#include "GameFramework/Actor.h"
#include "InteractiveLightSwitch.generated.h"

/*
 * AInteractiveLightSwitch - 可交互灯光开关器
 *
 * 打开 / 关闭灯光
 */
UCLASS()
class UNREALGAME_API AInteractiveLightSwitch : public AActor
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Light Switch", DisplayName="灯光开关静态网格体组件")
	UStaticMeshComponent* MeshComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Light", DisplayName="被控制灯")
	TArray<AInteractiveLight*> Lights;

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Light State", DisplayName="灯光开关的打开/关闭状态", meta=(AllowPrivateAccess))
	ELightState LightState = ELightState::TurnOff;
	
public:	
	// Sets default values for this actor's properties
	AInteractiveLightSwitch();

	UFUNCTION(BlueprintCallable, Category="Light Switch", DisplayName="切换开关")
	void Switch();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif

	
	UFUNCTION(BlueprintCallable, Category="Light Control", DisplayName="打开")
	virtual void TurnOn();

	UFUNCTION(BlueprintCallable, Category="Light Control", DisplayName="关闭")
	virtual void TurnOff();

public:	
	FORCEINLINE UStaticMeshComponent* GetMeshComponent() const { return MeshComponent; }
};


