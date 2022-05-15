// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "Components/Button.h"
#include "MultiplayerSessionSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"

void UMenu::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch, FString LobbyPath)
{
	PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);

	NumPublicConnections = NumberOfPublicConnections;
	MatchType = TypeOfMatch;
	
	AddToViewport();

	SetVisibility(ESlateVisibility::Visible);

	bIsFocusable = true;


	UWorld* World = GetWorld();
	if(World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();

		if(PlayerController)
		{
			FInputModeUIOnly InputModeUIOnly;
			InputModeUIOnly.SetWidgetToFocus(TakeWidget());
			InputModeUIOnly.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

			// 设置输入模式
			PlayerController->SetInputMode(InputModeUIOnly);

			// 显示鼠标
			PlayerController->SetShowMouseCursor(true);
		}
	}

	UGameInstance* GameInstance = GetGameInstance();
	if(GameInstance)
	{
		MultiplayerSessionSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionSubsystem>();
	}

	if (MultiplayerSessionSubsystem)
	{
		MultiplayerSessionSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSessionComplete);
		MultiplayerSessionSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &UMenu::OnFindSessionComplete);
		MultiplayerSessionSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &UMenu::OnJoinSessionComplete);
		MultiplayerSessionSubsystem->MultiplayerOnDestroySessiomComplete.AddDynamic(this, &ThisClass::UMenu::OnDestroySessionComplete);
		MultiplayerSessionSubsystem->MultiplayerOnStartSessiomComplete.AddDynamic(this, &ThisClass::UMenu::OnStartSessionComplete);
	}
}

bool UMenu::Initialize()
{
	if(!Super::Initialize())
	{
		return false;
	}

	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);
	}

	if(JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
	}
		
	return true;
}

void UMenu::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	MenuTearDown();

	Super::OnLevelRemovedFromWorld(InLevel, InWorld);
}

void UMenu::OnCreateSessionComplete(bool bWasSuccessful)
{
	if(bWasSuccessful)
	{
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.0f,
				FColor::Yellow,
				FString::Printf(TEXT("Session Created Successfully!"))
			);
		}
		// TODO 创建完 Session，切换地图的逻辑不应该在该处，而是在创建 Session 成功的回调函数中实现
		// 是否是游戏设计需要?
		// 当创建完成会话后，玩家进入关卡等待，如果会话创建成功，那么就等待其他玩家连接;
		// 如果创建失败，那么提示信息，让玩家自己决定如何处理，而不是不能让玩家不能创建房间。
		UWorld* World = GetWorld();


		// 是否可以设计成创建房间，等待创建会话，当会话创建成功时提示会话创建成功，当创建失败时提示创建失败，变成单机模式?
		if(World)
		{
			// 切换地图，并监听
			World->ServerTravel(PathToLobby);
		}
		
	}
	else
	{
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.0f,
				FColor::Yellow,
				FString::Printf(TEXT("Session Created Failure!"))
			);
		}

		// 重新启用 HostButton
		HostButton->SetIsEnabled(true);
	}
}

void UMenu::OnFindSessionComplete(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{

	if(MultiplayerSessionSubsystem == nullptr)
	{
		return;
	}
	
	for (FOnlineSessionSearchResult Result : SessionResults)
	{
		FString MatchTypeValue;
		Result.Session.SessionSettings.Get(FName("MatchType"), MatchTypeValue);

		if(MatchTypeValue == MatchType)
		{
			MultiplayerSessionSubsystem->JoinSession(Result);

			// TODO 暂时处理：立刻连接到可搜索到的第一个会话。
			return;
		}
	}

	if (!bWasSuccessful || SessionResults.Num() == 0)
	{
		// 如果查找会话失败，或会话数量 = 0，重新启用 JoinSession 按钮
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::OnJoinSessionComplete(EOnJoinSessionCompleteResult::Type Result)
{

	// TODO 应该判断 Result 的结果，然后选择对应的处理逻辑??? 如果 Result 的结果是失败，那么应该不能切换到指定的地图
	
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();

		if (SessionInterface.IsValid())
		{
			FString Address;

			// TODO 会话中是否会存在相同名称的会话呢? 还是在会话创建的时候，如果存在相同名称的会话会创建失败呢?
			// TODO 在这里统一使用 NAME_GameSession 获取会话的逻辑是什么，根据什么依据获取会话，名称还是?
			SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);

			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
			if (PlayerController)
			{
				PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
			}
		}
	}

	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::OnDestroySessionComplete(bool bWasSuccessful)
{
	
}

void UMenu::OnStartSessionComplete(bool bWasSuccessful)
{
	
}

void UMenu::HostButtonClicked()
{
	HostButton->SetIsEnabled(false);
	
	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.0f,
			FColor::Yellow,
			FString(TEXT("Host Button Clicked!"))
		);
	}

	if (MultiplayerSessionSubsystem)
	{
		MultiplayerSessionSubsystem->CreateSession(NumPublicConnections, MatchType);
	}
}

void UMenu::JoinButtonClicked()
{
	JoinButton->SetIsEnabled(false);
	
	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.0f,
			FColor::Yellow,
			FString(TEXT("Join Button Clicked!"))
		);
	}

	if (MultiplayerSessionSubsystem)
	{
		MultiplayerSessionSubsystem->FindSessions(10000);
	}
}

void UMenu::MenuTearDown()
{
	// 从父类中移除
	RemoveFromParent();

	UWorld* World = GetWorld();

	if(World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();

		FInputModeGameOnly InputModeData;

		PlayerController->SetInputMode(InputModeData);

		PlayerController->SetShowMouseCursor(false);
	}
}
