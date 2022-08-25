// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnrealPlayerController.h"
#include "BlasterPlayerController.generated.h"

enum class EPlayerInputMappingContent : uint8;
/**
 * 
 */
UCLASS()
class UNREALGAME_API ABlasterPlayerController : public AUnrealPlayerController
{
	GENERATED_BODY()


public:

	ABlasterPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// 显示生命值
	void SetHUDHealth(float Health, float MaxHealth);

	// 显示得分
	void SetHUDScore(float Score);

	// 显示死亡数
	void SetHUDDefeats(int32 Defeats);

	// 显示武器当前子弹
	void SetHUDWeaponAmmo(int32 Ammo);

	// 显示拥有的子弹
	void SetHUDCarriedAmmo(int32 CarriedAmmo);

	// 比赛倒计时
	void SetHUDMatchCountdown(float CountdownTime);

	void SetHUDAnnouncementCountdown(float CountdownTime);

	virtual void OnPossess(APawn* InPawn) override;

	virtual void Tick(float DeltaTime) override;

	// 同步服务器时间
	virtual float GetServerTime();

	virtual void ReceivedPlayer() override;

	void OnMatchStateSet(FName State);

	void HandleMatchHasStarted();

	void HandleCooldown();

protected:

	virtual void BeginPlay() override;

	void SetHUDTime();

	/*
	 * 同步客户端和服务器之间的时间
	 */

	// 请求当前服务器时间，传递给客户端发送请求时的时间
	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);

	// 向客户端报告当前服务器时间以响应 ServerRequestServerTime
	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceivedClientRequest);

	// 客户端与服务器时间的差别
	float ClientServerDelta = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Time")
	float TimeSyncFrequency = 5.0f;

	float TimeSyncRunningTime = 0.0f;

	void CheckTimeSync(float DeltaTime);

	void PollInit();

	UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();

	UFUNCTION(Client, Reliable) 
	void ClientJoinMidgame(FName StateOfMatch, float Warmup, float Match, float Cooldown, float StartingTime);

	void HighPingWarning();
	
private:

	UPROPERTY()
	class ABlasterHUD* BlasterHUD;

	UPROPERTY()
	class ABlasterGameMode* BlasterGameMode;

	float LevelStartingTime = 0.0f;

	float MatchTime = 0.0f;

	float WarmupTime = 0.0f;

	float CooldownTime = 0.0f;

	uint32 CountdownInt = 0;

	UPROPERTY(ReplicatedUsing="OnRep_MatchState")
	FName MatchState;

	UFUNCTION()
	void OnRep_MatchState();

	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;

	bool bInitializeCharacterOverlay = false;

	float HUDHealth;
	float HUDMaxHealth;
	float HUDScore;
	float HUDDefeats;

	float HighPingRunningTime = 0.0f;

	UPROPERTY(EditAnywhere)
	float HighPingDuration = 5.0f;

	UPROPERTY(EditAnywhere)
	float CheckPingFrequency = 20.0f;

	UPROPERTY(EditAnywhere)
	float HighPingThreshold = 50.0f;

	// Debug
	TArray<EPlayerInputMappingContent> PlayerInputMappingContents;

public:

	UFUNCTION(Exec, Category="Shot", DisplayName="控制台执行创建摄像纹理")
	void ConsoleCreateShotTexture();

	UFUNCTION(Exec, Category="Player Input", DisplayName="控制台切换输入映射上下文")
	void ConsoleSwitchInputMappingContext(int32 Index);
};
