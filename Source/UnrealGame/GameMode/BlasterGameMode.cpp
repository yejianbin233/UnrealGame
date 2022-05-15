// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealGame/GameMode/BlasterGameMode.h"

#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "UnrealGame/Character/BlasterCharacter.h"
#include "UnrealGame/PlayerController/BlasterPlayerController.h"
#include "UnrealGame/PlayerState/BlasterPlayerState.h"

void ABlasterGameMode::PlayerEliminated(ABlasterCharacter* ElimmedCharacter, ABlasterPlayerController* VictimController,
                                        ABlasterPlayerController* AttackerController)
{

	ABlasterPlayerState* AttackerPlayerState = AttackerController->GetPlayerState<ABlasterPlayerState>();

	ABlasterPlayerState* VictimPlayerState = VictimController->GetPlayerState<ABlasterPlayerState>();

	if (AttackerPlayerState && VictimPlayerState && AttackerPlayerState != VictimPlayerState)
	{
		AttackerPlayerState->AddToScore(1.0f);
	}

	if (VictimPlayerState)
	{
		VictimPlayerState->AddToDefeats(1);
	}
	
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Elim();
	}
}

void ABlasterGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
	if (ElimmedCharacter)
	{
		// 将角色与控制器分离
		ElimmedCharacter->Reset();

		// 销毁角色
		ElimmedCharacter->Destroy();

	}

	if (ElimmedController)
	{
		TArray<AActor*> PlayerStarts; 
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);

		int32 Selection = FMath::RandRange(0, PlayerStarts.Num()-1);
		// 尝试在指定位置(PlayerStart)生成 Pawn 
		RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);
	}
}
