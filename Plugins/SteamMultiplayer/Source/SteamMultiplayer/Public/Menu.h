// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Menu.generated.h"

/**
 * 
 */
UCLASS()
class STEAMMULTIPLAYER_API UMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	// 菜单设置
	void MenuSetup(int32 NumberOfPublicConnections = 4
		, FString TypeOfMatch = FString(TEXT("FreeForAll"))
		, FString LobbyPath = FString(TEXT("/Game/OnlineUnrealGame/Maps/Lobby")));

protected:

	// 菜单初始化
	virtual bool Initialize() override;

	// 当切换到不同的关卡时，所有 Widget 控件都会调用 OnLevelRemovedFromWorld 函数
	virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) override;

	// 自定义委托回调
	UFUNCTION()
	void OnCreateSessionComplete(bool bWasSuccessful);

	void OnFindSessionComplete(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);

	void OnJoinSessionComplete(EOnJoinSessionCompleteResult::Type Result);

	UFUNCTION()
	void OnDestroySessionComplete(bool bWasSuccessful);
	
	UFUNCTION()
	void OnStartSessionComplete(bool bWasSuccessful);
	
private:

	
	UPROPERTY(meta=(BindWidget)) // 将 HostButton 与 Widget 中的控件绑定，必须名称相同才能完成绑定
	class UButton* HostButton;

	UPROPERTY(meta=(BindWidget))
	UButton* JoinButton;

	UFUNCTION()
	void HostButtonClicked();

	UFUNCTION()
	void JoinButtonClicked();

	// 菜单撤除
	void MenuTearDown();

	class UMultiplayerSessionSubsystem* MultiplayerSessionSubsystem;

	int32 NumPublicConnections{4};

	FString MatchType{TEXT("FreeForAll")};

	FString PathToLobby{TEXT("")};
};
