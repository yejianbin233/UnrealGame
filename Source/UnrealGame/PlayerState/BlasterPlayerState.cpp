// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealGame/PlayerState/BlasterPlayerState.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "UnrealGame/Character/BlasterCharacter.h"
#include "UnrealGame/PlayerController/BlasterPlayerController.h"

void ABlasterPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterPlayerState, Defeats);
}

void ABlasterPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

 	Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;

	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->GetController()) : Controller;

		if (Controller)
		{
			Controller->SetHUDScore(GetScore());
		}
	}
	
}

void ABlasterPlayerState::OnRep_Defeats()
{
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->GetController()) : Controller;

		if (Controller)
		{
			Controller->SetHUDDefeats(Defeats);
		}
	}
}

void ABlasterPlayerState::AddToScore(float ScoreAmount)
{
	ScoreAmount = GetScore() + ScoreAmount;
	SetScore(ScoreAmount);

	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->GetController()) : Controller;

		if (Controller)
		{
			Controller->SetHUDScore(GetScore());
		}
	}
	
}

void ABlasterPlayerState::AddToDefeats(int32 DefeatsAmount)
{
	Defeats = Defeats + DefeatsAmount;

	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->GetController()) : Controller;

		if (Controller)
		{
			Controller->SetHUDDefeats(Defeats);
		}
	}
}



