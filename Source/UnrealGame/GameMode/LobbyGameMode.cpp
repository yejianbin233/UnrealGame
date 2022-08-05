// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();

	if(NumberOfPlayers == 1)
	{
		UWorld* World = GetWorld();

		if (World)
		{
			// 使用无缝过渡(需要设置无缝过渡地图)
			bUseSeamlessTravel = true;
			// World->ServerTravel(FString("Game/UnrealGame/Maps/BlasterMap?listen"));
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, TravelUrl);
			
			bool Result = World->ServerTravel(TravelUrl);
			if (Result)
			{
				GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, FString::Printf(TEXT("Travel Succeed!")));
			} else
			{
				GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, FString::Printf(TEXT("Travel Fail...")));
			}
		}
	}
}
