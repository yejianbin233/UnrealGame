// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealGame/PlayerController/BlasterPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/PlayerState.h"
#include "UnrealGame/PlayerState/BlasterPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "UnrealGame/Character/BlasterCharacter.h"
#include "UnrealGame/GameMode/BlasterGameMode.h"
#include "UnrealGame/HUD/Announcement.h"
#include "UnrealGame/HUD/BlasterHUD.h"
#include "UnrealGame/HUD/CharacterOverlay.h"

ABlasterPlayerController::ABlasterPlayerController(const FObjectInitializer& ObjectInitializer)
{
	PlayerInputMappingContents.Add(EPlayerInputMappingContent::Shooter);
	PlayerInputMappingContents.Add(EPlayerInputMappingContent::Camera);
}

void ABlasterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ServerCheckMatchState();

	BlasterHUD = Cast<ABlasterHUD>(GetHUD());

	ServerCheckMatchState();
}

void ABlasterPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetHUDTime();

	CheckTimeSync(DeltaTime);

	PollInit();

	HighPingRunningTime += DeltaTime;

	if (HighPingRunningTime > CheckPingFrequency)
	{
		ABlasterPlayerState* BlasterPlayerState = PlayerState == nullptr ? GetPlayerState<ABlasterPlayerState>() : nullptr;

		if (PlayerState)
		{

			// PlayerState->GetPing() * 4 > HighPingThreshold; // UE 将 Ping 除以 4，以将其压缩，因此为了得到正确的 Ping 需要乘以 4.
			if (PlayerState->GetPingInMilliseconds() * 4 > HighPingThreshold)
			{
				// 高 Ping 处理
			}; 
		}

		HighPingRunningTime = 0.0f;
	}
}

void ABlasterPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterPlayerController, MatchState);
}

void ABlasterPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;


	bool bHUDValid = BlasterHUD
		&& BlasterHUD->CharacterOverlay
		&& BlasterHUD->CharacterOverlay->HealthBar
		&& BlasterHUD->CharacterOverlay->HealthText;

	if (bHUDValid)
	{
		const float HealthPercent = Health / MaxHealth;
		BlasterHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);

		FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		BlasterHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
	else
	{
		bInitializeCharacterOverlay = true;
		HUDHealth = Health;
		HUDMaxHealth = MaxHealth;
	}
}

void ABlasterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(InPawn);

	// 当控制器重新拥有 Character 时，初始化 Health
	if (BlasterCharacter)
	{
		SetHUDHealth(BlasterCharacter->GetHealth(), BlasterCharacter->GetMaxHealth());
	}
	
}

void ABlasterPlayerController::SetHUDScore(float Score)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;


	bool bHUDValid = BlasterHUD
		&& BlasterHUD->CharacterOverlay
		&& BlasterHUD->CharacterOverlay->ScoreAmount;

	if (bHUDValid)
	{
		FString ScoreAmountText = FString::Printf(TEXT("%d"), FMath::CeilToInt(Score));
		
		BlasterHUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScoreAmountText));
	}
	else
	{
		bInitializeCharacterOverlay = true;
		HUDScore = Score;
	}
}

void ABlasterPlayerController::SetHUDDefeats(int32 Defeats)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;


	bool bHUDValid = BlasterHUD
		&& BlasterHUD->CharacterOverlay
		&& BlasterHUD->CharacterOverlay->DefeatAmount;

	if (bHUDValid)
	{
		FString DefeatAmountText = FString::Printf(TEXT("%d"), Defeats);
		
		BlasterHUD->CharacterOverlay->DefeatAmount->SetText(FText::FromString(DefeatAmountText));
	}
	else
	{
		bInitializeCharacterOverlay = true;
		HUDDefeats = Defeats;
	}
}

void ABlasterPlayerController::SetHUDWeaponAmmo(int32 Ammo)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;


	bool bHUDValid = BlasterHUD
		&& BlasterHUD->CharacterOverlay
		&& BlasterHUD->CharacterOverlay->WeaponAmmoAmount;

	if (bHUDValid)
	{
		FString AmmoAmountText = FString::Printf(TEXT("%d"), Ammo);
		
		BlasterHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(AmmoAmountText));
	}
}

void ABlasterPlayerController::SetHUDCarriedAmmo(int32 CarriedAmmo)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;


	bool bHUDValid = BlasterHUD
		&& BlasterHUD->CharacterOverlay
		&& BlasterHUD->CharacterOverlay->CarriedAmmoAmount;

	if (bHUDValid)
	{
		FString CarriedAmmoAmountText = FString::Printf(TEXT("%d"), CarriedAmmo);
		
		BlasterHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(CarriedAmmoAmountText));
	}
}

void ABlasterPlayerController::SetHUDMatchCountdown(float CountdownTime)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	
	bool bHUDValid = BlasterHUD
		&& BlasterHUD->CharacterOverlay
		&& BlasterHUD->CharacterOverlay->MatchCountdownText;

	if (bHUDValid)
	{
		if (CountdownTime < 0.0f)
		{
			BlasterHUD->CharacterOverlay->MatchCountdownText->SetText(FText());
			return;
		}
		
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.0f);
		int32 Seconds = FMath::FloorToInt(CountdownTime - Minutes * 60.0f);
		
		// int32 T = Minutes % Seconds; % 需要运算值都为整数。


		// %02d - 将数字格式化为两位，左侧填充零
		FString MatchCountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		
		BlasterHUD->CharacterOverlay->MatchCountdownText->SetText(FText::FromString(MatchCountdownText));
	}
}

void ABlasterPlayerController::SetHUDAnnouncementCountdown(float CountdownTime)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;


	bool bHUDValid = BlasterHUD
		&& BlasterHUD->Announcement
		&& BlasterHUD->Announcement->WarmupTime;

	if (bHUDValid)
	{
		if (CountdownTime < 0.0f)
		{
			BlasterHUD->Announcement->WarmupTime->SetText(FText());
			return;
		}
		
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.0f);
		int32 Seconds = FMath::FloorToInt(CountdownTime - Minutes * 60.0f);
		
		// int32 T = Minutes % Seconds; % 需要运算值都为整数。


		// %02d - 将数字格式化为两位，左侧填充零
		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		
		BlasterHUD->Announcement->WarmupTime->SetText(FText::FromString(CountdownText));
	}
}

void ABlasterPlayerController::SetHUDTime()
{
	float TimeLeft = 0.0f;
	if (MatchState == MatchState::WaitingToStart)
	{
		TimeLeft = WarmupTime - GetServerTime() + LevelStartingTime;
	}
	else if(MatchState == MatchState::InProgress)
	{
		TimeLeft = WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	}
	else if (MatchState == MatchState::Cooldown)
	{
		TimeLeft = CooldownTime + WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	}
	
	
	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);

	if (HasAuthority())
	{
		BlasterGameMode = BlasterGameMode == nullptr ? Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this)) : BlasterGameMode;

		if (BlasterGameMode)
		{
			SecondsLeft = FMath::CeilToInt(BlasterGameMode->GetCountdownTime());
		}
	}

	if (CountdownInt != SecondsLeft)
	{
		SetHUDMatchCountdown(MatchTime - GetServerTime());
		
		if (MatchState == MatchState::WaitingToStart || MatchState == MatchState::Cooldown)
		{
			SetHUDAnnouncementCountdown(TimeLeft);
		}

		if (MatchState == MatchState::InProgress)
		{
			SetHUDMatchCountdown(TimeLeft);
		}
		
	}
	
	CountdownInt = SecondsLeft;
	
}

void ABlasterPlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();

	ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

void ABlasterPlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest,
	float TimeServerReceivedClientRequest)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;

	float CurrentServerTime = TimeServerReceivedClientRequest + (0.5f * RoundTripTime);

	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

float ABlasterPlayerController::GetServerTime()
{
	if (HasAuthority())
	{
		return GetWorld()->GetTimeSeconds();
	}
	return GetWorld()->GetTimeSeconds() + ClientServerDelta;	
}

void ABlasterPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

void ABlasterPlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.0f;
	}
}

void ABlasterPlayerController::OnMatchStateSet(FName State)
{
	MatchState = State;

	if (MatchState == MatchState::WaitingToStart)
	{
		
	}

	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if(MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
	
}

void ABlasterPlayerController::OnRep_MatchState()
{
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if(MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void ABlasterPlayerController::ConsoleCreateShotTexture()
{
	UUnrealCameraComponent::CreateShotTexture();
}

void ABlasterPlayerController::ConsoleSwitchInputMappingContext(int32 Index)
{
	if (Index < 0 || Index >= PlayerInputMappingContents.Num())
	{
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, FString::Printf(TEXT("下标越界，应输入[0, %d)的整数"), PlayerInputMappingContents.Num()));
		return;
	}
	if (ABlasterCharacter* PlayerCharacter = Cast<ABlasterCharacter>(GetPawn()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(this->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();

			EPlayerInputMappingContent IndexContent = PlayerInputMappingContents[Index];
			switch (IndexContent)
			{
				case EPlayerInputMappingContent::Shooter :
					{
						GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Green, FString::Printf(TEXT("切换到射手输入模式")));
						Subsystem->AddMappingContext(PlayerCharacter->ShootGameInputMappingContext, 0);
						break;
					}
				case EPlayerInputMappingContent::Camera :
					{
						GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Green, FString::Printf(TEXT("切换到摄像机玩家输入模式")));
						Subsystem->AddMappingContext(PlayerCharacter->GetPlayerCameraComponent()->CameraModeInputMappingContext, 0);
						break;
					}
				default:
						break;
					;
			}
		}
	}
}

void ABlasterPlayerController::PollInit()
{
	if (CharacterOverlay == nullptr)
	{
		if (BlasterHUD && BlasterHUD->CharacterOverlay)
		{
			CharacterOverlay = BlasterHUD->CharacterOverlay;

			if (CharacterOverlay)
			{
				SetHUDHealth(HUDHealth, HUDMaxHealth);
				SetHUDScore(HUDScore);
				SetHUDDefeats(HUDDefeats);
			}
		}
	}	
}

void ABlasterPlayerController::HighPingWarning()
{
	 
}

void ABlasterPlayerController::HandleMatchHasStarted()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	if (BlasterHUD)
	{
		BlasterHUD->AddCharacterOverlay();

		if (BlasterHUD->Announcement)
		{
			// 隐藏 HUD
			BlasterHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void ABlasterPlayerController::HandleCooldown()
{
	// BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	//
	// if (BlasterHUD)
	// {
	// 	BlasterHUD->CharacterOverlay->RemoveFromParent();
	//
	// 	bool bHUDValid = BlasterHUD->Announcement
	// 		&& BlasterHUD->Announcement->AnnouncementText
	// 		&& BlasterHUD->Announcement->InfoText;
	//
	// 	if (bHUDValid)
	// 	{
	// 		// 隐藏 HUD
	// 		BlasterHUD->Announcement->SetVisibility(ESlateVisibility::Visible);
	//
	// 		FString AnnouncementText("New Match Start In:");
	// 		BlasterHUD->Announcement->AnnouncementText->SetText(FText::FromString(AnnouncementText));
	//
	// 		BlasterHUD->Announcement->InfoText->SetText(FText());
	// 	}
	// }
	//
	// ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(GetPawn());
	//
	// if (BlasterCharacter && BlasterCharacter->GetCombatComponent())
	// {
	// 	// BlasterCharacter->bDisableGameplay = true;
	//
	// 	// BlasterCharacter->GetCombat()->FireButtonPressed(false);
	// 	
	// }
}

void ABlasterPlayerController::ServerCheckMatchState_Implementation()
{
	ABlasterGameMode* GameMode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (GameMode)
	{
		WarmupTime = GameMode->WarmupTime;
		MatchTime = GameMode->MatchTime;
		CooldownTime = GameMode->CooldownTime;
		LevelStartingTime = GameMode->LevelStartingTime;
		MatchState = GameMode->GetMatchState();

		ClientJoinMidgame(MatchState, WarmupTime, MatchTime, CooldownTime, LevelStartingTime);

		
	}
}

void ABlasterPlayerController::ClientJoinMidgame_Implementation(FName StateOfMatch, float Warmup, float Match, float Cooldown, float StartingTime)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	CooldownTime = Cooldown;
	LevelStartingTime = StartingTime;
	MatchState = StateOfMatch;

	OnMatchStateSet(MatchState);
	if (BlasterHUD && MatchState == MatchState::WaitingToStart)
	{
		BlasterHUD->AddAnnouncement();
	}
}