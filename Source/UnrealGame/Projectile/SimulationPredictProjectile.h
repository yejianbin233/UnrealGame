// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UnrealGame/Character/BlasterCharacter.h"
#include "SimulationPredictProjectile.generated.h"

UCLASS()
class UNREALGAME_API ASimulationPredictProjectile : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Component", DisplayName="子弹碰撞盒子组件")
	class UBoxComponent* CollisionBox;

	// 重叠的碰撞盒子
	TArray<class AReapperCollisionBox*> OverlapReapperCollisionBoxes;

	// TODO 模拟子弹需要标识当次模拟击中生成的重新碰撞盒子
	FName SimulationName;

	FTimerHandle PredictProjectileTimerHandle;
	
public:	
	// Sets default values for this actor's properties
	ASimulationPredictProjectile();

	void Simulation(float SimulationTime, FVector NewCollisionExtent);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void SimulationEnded();
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
