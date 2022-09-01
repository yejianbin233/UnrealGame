// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UnrealGame/Interfaces/UnrealInterface.h"
#include "InteractiveDoor.generated.h"

UCLASS()
class UNREALGAME_API AInteractiveDoor : public AActor, public IInteractiveInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Component", DisplayName="门框组件")
	UStaticMeshComponent* DoorFrameComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Component", DisplayName="门扇组件")
	UStaticMeshComponent* DoorLeafComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Door", DisplayName="打开门幅度")
	float OpenDoorAngle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Door", DisplayName="门开关状态")
	bool bIsOpen = false;

	UPROPERTY(BlueprintReadWrite, Category="Door", DisplayName="门是否正在运动")
	bool bIsPlaying;

public:	
	// Sets default values for this actor's properties
	AInteractiveDoor();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Interactive", DisplayName="交互")
	void Interactive() override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
