// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ConsoleVariableActor.generated.h"

UCLASS()
class UNREALGAME_API AConsoleVariableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AConsoleVariableActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	static float GetShowLocalRoleDebugInfo();
	static float GetClientServerTimeSync();
	static float GetDetectPickableObjectDebug();
	static float GetEnterOrLeavePickableItemLog();
	static float GetEnterCameraDebugMode();
	static float GetShowCameraDebugLog();
	static float GetShowBackpackDebugLog();

};
