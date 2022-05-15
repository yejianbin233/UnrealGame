// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"


UMultiplayerSessionSubsystem::UMultiplayerSessionSubsystem():
	CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &UMultiplayerSessionSubsystem::OnCreateSessionComplete))
	, FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &UMultiplayerSessionSubsystem::OnFindSessionsComplete))
	, JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &UMultiplayerSessionSubsystem::OnJoinSessionComplete))
	, DestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &UMultiplayerSessionSubsystem::OnDestroySessionComplete))
	, StartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &UMultiplayerSessionSubsystem::OnStartSessiomComplete))
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();

	if(OnlineSubsystem)
	{
		SessionInterface = OnlineSubsystem->GetSessionInterface();
	}
}

void UMultiplayerSessionSubsystem::CreateSession(int32 NumPublicConnections, FString MatchType)
{
	if(!SessionInterface.IsValid())
	{
		return;
	}

	FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession)
	{
		bCreateSessionOnDestroy = true;
		LastNumPublicConnections = NumPublicConnections;
		LastMatchType = MatchType;
		
		// 一个应用只需要创建一个 Session 即可，即使不同机器的使用相同的 Session 名称。
		// SessionInterface->DestroySession(NAME_GameSession);
		DestroySession();

		// TODO 这里是否是需要 return ?
	}

	CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	SessionSettings = MakeShareable(new FOnlineSessionSettings());

	// 是否是局域网(游戏内定义? 还是什么条件决定?)
	SessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	SessionSettings->NumPublicConnections = NumPublicConnections;

	// 是否允许在进行中加入???
	SessionSettings->bAllowJoinInProgress = true;

	// bShouldAdvertise 的作用是什么? 应该广告?? 是否在在线服务上公开宣传
	SessionSettings->bShouldAdvertise = true;

	// 是否仅寻找在线会话
	SessionSettings->bUsesPresence = true;

	// 通过在线服务和 Ping 数据进行宣传，MatchType(匹配模式)
	SessionSettings->Set(FName("MatchType"), MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	// 用于防止不同的构建在搜索过程中相互看到，应该设置为一个随机值
	SessionSettings->BuildUniqueId = 1;
	
	// TODO 验证：是否在机器内部存在多个 LocalPlayer，且该机器的玩家的 LocalPlayer 一定可以通过 GetFirstLocalPlayerFormController 找到???
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	
	bool CreateResult = SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *SessionSettings);

	if(!CreateResult)
	{
		// 如果创建失败，那么立刻将创建会话绑定的回调清除
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);

		// 创建失败，将通过委托广播失败
		MultiplayerOnCreateSessionComplete.Broadcast(false);
	}
}

void UMultiplayerSessionSubsystem::FindSessions(int32 MaxSearchResults)
{
	if(!SessionInterface.IsValid())
	{
		return;
	}

	FindSessionsCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->MaxSearchResults = MaxSearchResults;
	SessionSearch->bIsLanQuery = false;

	// QuerySettings.Set() 在会话中的作用是什么? 搜索时会对相应的值进行比较后才能搜索到吗?
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	bool FindResult = SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());

	if (!FindResult)
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);

		MultiplayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
	}
}

void UMultiplayerSessionSubsystem::JoinSession(const FOnlineSessionSearchResult& SearchResult)
{
	if(!SessionInterface.IsValid())
	{
		MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}

	JoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	bool JoinSessionResult = SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SearchResult);

	if (!JoinSessionResult)
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);

		MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
	}
}

void UMultiplayerSessionSubsystem::DestroySession()
{
	if (!SessionInterface)
	{
		MultiplayerOnDestroySessiomComplete.Broadcast(false);
		return;
	}

	DestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);

	bool DestroySessionResult = SessionInterface->DestroySession(NAME_GameSession);
	if (!DestroySessionResult)
	{
		MultiplayerOnDestroySessiomComplete.Broadcast(false);
	}
	else
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
		
		MultiplayerOnDestroySessiomComplete.Broadcast(true);
	}
}

void UMultiplayerSessionSubsystem::StartSession()
{
	
}

void UMultiplayerSessionSubsystem::OnCreateSessionComplete(FName InSessionName, bool bWasSuccessful)
{
	if(SessionInterface)
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}

	// 广播创建 Session 的结果
	MultiplayerOnCreateSessionComplete.Broadcast(bWasSuccessful);
}

void UMultiplayerSessionSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	if(SessionInterface)
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
	}

	if (SessionSearch->SearchResults.Num() <= 0)
	{
		MultiplayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
	}
	else
	{
		MultiplayerOnFindSessionsComplete.Broadcast(SessionSearch->SearchResults, bWasSuccessful);
	}
}

void UMultiplayerSessionSubsystem::OnJoinSessionComplete(FName InSessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (SessionInterface)
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);	
	}

	MultiplayerOnJoinSessionComplete.Broadcast(Result);
}

void UMultiplayerSessionSubsystem::OnDestroySessionComplete(FName InSessionName, bool bWasSuccessful)
{
	if (SessionInterface)
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
	}

	if (bWasSuccessful && bCreateSessionOnDestroy)
	{
		// 重置
		bCreateSessionOnDestroy = false;
		// TODO 为什么要继续创建? 因为唯一会触发的地方是在 CreateSession 时销毁已存在的会话，此刻创建那么 CreateSession 也会创建，不就创建了两个了吗???

		CreateSession(LastNumPublicConnections, LastMatchType);
	}

	MultiplayerOnDestroySessiomComplete.Broadcast(true);
}

void UMultiplayerSessionSubsystem::OnStartSessiomComplete(FName InSessionName, bool bWasSuccessful)
{
	
}

