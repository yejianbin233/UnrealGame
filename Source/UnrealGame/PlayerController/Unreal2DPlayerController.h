// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Unreal2DPlayerController.generated.h"


class AUnreal2DCharacter;

class AUnreal2DCameraActor;
/**
 * 
 */
UCLASS()
class UNREALGAME_API AUnreal2DPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Widget", DisplayName="选择角色控件类")
	TSubclassOf<UUserWidget> SelectCharacterWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera", DisplayName="玩家角色摄像机类")
	TSubclassOf<AUnreal2DCameraActor> PlayerCameraClass;

	UPROPERTY(BlueprintReadOnly, Category="Widget", DisplayName="选择角色控件")
	UUserWidget* SelectCharacterWidget;
	
public:

	virtual void BeginPlay() override;

	UFUNCTION(Client, Reliable, BlueprintCallable, DisplayName="初始化控件")
	void InitializedWidget();

	UFUNCTION(Server, Reliable, BlueprintCallable, DisplayName="创建玩家角色")
	void CreatePlayerCharacter(TSubclassOf<AUnreal2DCharacter> PlayerCharacterClass, FVector InSpawnLocation);

	UFUNCTION(Server, Reliable, BlueprintCallable, DisplayName="请求生成玩家角色")
	void RequestSpawnPlayerCharacter(TSubclassOf<AUnreal2DCharacter> PlayerCharacterClass);

private:

	UFUNCTION(BlueprintCallable, DisplayName="创建玩家角色摄像机")
	void CreatePlayerCamera();
};
