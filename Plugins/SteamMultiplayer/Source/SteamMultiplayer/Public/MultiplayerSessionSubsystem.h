// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MultiplayerSessionSubsystem.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnCreateSessionComplete, bool, bWasSuccessful);

// 动态多播委托的参数类型必须与蓝图兼容，因此如果委托的参数不是新类或新结构不能控制其是否与蓝图姜蓉，那么应声明为多播委托，而不是动态多播委托
DECLARE_MULTICAST_DELEGATE_TwoParams(FMultiplayerOnFindSessionsComplete, const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FMultiplayerOnJoinSessionComplete, EOnJoinSessionCompleteResult::Type Result);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnDestroySessiomComplete, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnStartSessiomComplete, bool, bWasSuccessful);
/**
 * 
 */
UCLASS()
class STEAMMULTIPLAYER_API UMultiplayerSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UMultiplayerSessionSubsystem();

	/*
	 * @Param NumPublicConnections - 公共连接数
	 * @Param MatchType - 匹配类型
	 */
	void CreateSession(int32 NumPublicConnections, FString MatchType);

	void FindSessions(int32 MaxSearchResults);

	void JoinSession(const FOnlineSessionSearchResult& SearchResult);

	void DestroySession();

	void StartSession();

	// 自定义委托
	FMultiplayerOnCreateSessionComplete MultiplayerOnCreateSessionComplete;

	FMultiplayerOnFindSessionsComplete MultiplayerOnFindSessionsComplete;

	FMultiplayerOnJoinSessionComplete MultiplayerOnJoinSessionComplete;

	FMultiplayerOnDestroySessiomComplete MultiplayerOnDestroySessiomComplete;
	
	FMultiplayerOnStartSessiomComplete MultiplayerOnStartSessiomComplete;
	
protected:
	/*
	 * 添加到在线会话接口委托列表中的委托的内部回调。
	 * 这个不需要在这个类之外调用。
	 */
	void OnCreateSessionComplete(FName InSessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName InSessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnDestroySessionComplete(FName InSessionName, bool bWasSuccessful);
	void OnStartSessiomComplete(FName InSessionName, bool bWasSuccessful);

private:
	IOnlineSessionPtr SessionInterface;

	TSharedPtr<FOnlineSessionSettings> SessionSettings;

	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	
	/*
	 * 添加到在线会话接口委托列表。
	 * Multiplayersession 子系统内部回调绑定到这些。
	 */
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FDelegateHandle CreateSessionCompleteDelegateHandle;
	
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FDelegateHandle FindSessionsCompleteDelegateHandle;
	
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	FDelegateHandle JoinSessionCompleteDelegateHandle;
	
	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	FDelegateHandle DestroySessionCompleteDelegateHandle;
	
	FOnStartSessionCompleteDelegate StartSessionCompleteDelegate;
	FDelegateHandle StartSessionCompleteDelegateHandle;

	bool bCreateSessionOnDestroy{ false };

	int32 LastNumPublicConnections;
	FString LastMatchType;
};
