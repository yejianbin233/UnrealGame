// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpecialGround.generated.h"

UCLASS()
class UNREALGAME_API ASpecialGround : public AActor
{
	GENERATED_BODY()

public:
	// TODO 暂不设计可下蹲跳跃
	
public:	
	// Sets default values for this actor's properties
	ASpecialGround();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
