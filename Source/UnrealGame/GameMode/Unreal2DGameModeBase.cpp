// Fill out your copyright notice in the Description page of Project Settings.


#include "Unreal2DGameModeBase.h"

#include "UnrealGame/Character/Unreal2DCharacter.h"
#include "UnrealGame/PlayerController/Unreal2DPlayerController.h"

void AUnreal2DGameModeBase::RequestSpawnPlayerCharacter(TSubclassOf<AUnreal2DCharacter> PlayerCharacterClass,
                                                        APlayerController* PlayerController)
{
	AUnreal2DPlayerController* Unreal2DPlayerController = Cast<AUnreal2DPlayerController>(PlayerController);
	if (Unreal2DPlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameMode RequestSpawn Location X%f - Y%f - Z%f"), SpawnPlayerLocation.X, SpawnPlayerLocation.Y, SpawnPlayerLocation.Z);
		Unreal2DPlayerController->CreatePlayerCharacter(PlayerCharacterClass, SpawnPlayerLocation);
	}
}
