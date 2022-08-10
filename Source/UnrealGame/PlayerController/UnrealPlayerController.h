// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UnrealPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class UNREALGAME_API AUnrealPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

protected:
	UPROPERTY(EditDefaultsOnly, Category="Time Sync", DisplayName="时间同步频率", meta=(UIMin=0))
	float TimeSyncFrequencey;

	UPROPERTY(EditDefaultsOnly, Category="Ping", DisplayName="客户端 Ping 值更新频率", meta=(UIMin=0))
	float ClientPingUpdateFrequencey;
	
	FTimerHandle TimeSyncHandle;
	
	FTimerHandle ClientPingUpdateHandle;

private:
	float ClientServerTimeDelta = 0.f;

	float ClientActualPingValue = 0.f;

public:
	UFUNCTION(BlueprintCallable, Category="Time Sync", DisplayName="客户端同步服务器时间")
	void ClientSyncServerTime();

	UFUNCTION(BlueprintCallable, Category="Ping", DisplayName="获取客户端实际 Ping 值")
	void UpdateClientActualPing();

	UFUNCTION(BlueprintCallable)
	void CancelPickItemPickup();

protected:
	
	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;

	UFUNCTION(Server, Reliable, Category="Time Sync", DisplayName="客户端请求服务器时间")
	void ClientRequestServerTime(float RequestTimeOfClient);

	UFUNCTION(Client, Reliable, Category="Time Sync", DisplayName="服务器回报服务器时间")
	void ServerReportServerTime(float RequestTimeOfClient, float ReportTimeOfServer);

private:


public:

	UFUNCTION(BlueprintCallable, Category="Time Sync", DisplayName="获取客户端与服务器之间的时间差值")
	FORCEINLINE float GetClientServerTimeDelta() const { return ClientServerTimeDelta; };

	UFUNCTION(BlueprintCallable, Category="Ping", DisplayName="获取客户端实际 Ping 值")
	FORCEINLINE float GetClientActualPing() const { return ClientActualPingValue; };
};
