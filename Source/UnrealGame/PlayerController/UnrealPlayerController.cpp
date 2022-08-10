// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealPlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "UnrealGame/Backpack/ItemBase.h"
#include "UnrealGame/Character/BlasterCharacter.h"
#include "UnrealGame/ConsoleVariable/ConsoleVariableActor.h"

void AUnrealPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		UE_LOG(LogTemp, Log, TEXT("Client Sync Server Time TimerHandle Ready!"));
		GetWorld()->GetTimerManager().SetTimer(TimeSyncHandle, this, &ThisClass::ClientSyncServerTime, TimeSyncFrequencey, true);

		UE_LOG(LogTemp, Log, TEXT("Client Update Ping TimerHandle Ready!"));
		GetWorld()->GetTimerManager().SetTimer(ClientPingUpdateHandle, this, &ThisClass::UpdateClientActualPing, ClientPingUpdateFrequencey, true);
	}
}

void AUnrealPlayerController::BeginDestroy()
{
	Super::BeginDestroy();
}

void AUnrealPlayerController::ClientSyncServerTime()
{
	const float RequestTimeOfClient = GetWorld()->GetTimeSeconds();
	
	ClientRequestServerTime(RequestTimeOfClient);
}

void AUnrealPlayerController::UpdateClientActualPing()
{
	// 在玩家控制器中获取玩家状态
	PlayerState = PlayerState == nullptr ? GetPlayerState<APlayerState>() : PlayerState;

	if (PlayerState)
	{
		UE_LOG(LogTemp, Log, TEXT("Get PlayerState In Local PlayerControlller."));

		// UE 在将 Ping 值传输时进行了特殊处理，传输后的值仅为原始的 1/4，因此需要乘以 4.
		const float CompressedPing = PlayerState->GetCompressedPing();
		ClientActualPingValue = CompressedPing * 4;
		ABlasterCharacter::DisplayRole(GetLocalRole());
		UE_LOG(LogTemp, Log, TEXT("Origin Ping: %f, Client Actual Ping: %f"), CompressedPing, ClientActualPingValue);
	}
}

void AUnrealPlayerController::CancelPickItemPickup()
{
	TArray<AActor*> Items;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AItemBase::StaticClass(), Items);
	for (auto Item : Items)
	{
		AItemBase* ItemBase = Cast<AItemBase>(Item);

		if (ItemBase)
		{
			ItemBase->CC_CanclePickedUpHandle();
			UE_LOG(LogTemp, Warning, TEXT("CanclePickedUpHandle Item: %s"), *Item->GetName());
		}
	}
}

void AUnrealPlayerController::ClientRequestServerTime_Implementation(float RequestTimeOfClient)
{
	const float ReportTimeOfServer = GetWorld()->GetTimeSeconds();
	
	ServerReportServerTime(RequestTimeOfClient, ReportTimeOfServer);
}

void AUnrealPlayerController::ServerReportServerTime_Implementation(float RequestTimeOfClient, float ReportTimeOfServer)
{
	// 往返时间
	const float CurrentClientTimeSeconds = GetWorld()->GetTimeSeconds();
	const float RoundTripTime = CurrentClientTimeSeconds - RequestTimeOfClient;

	// 服务器时间返回经过时间
	const float ServerReturnTime = CurrentClientTimeSeconds - ReportTimeOfServer;

	// 判断往/返时间是否相同
	const float HalfRoundTripTime = RoundTripTime / 2;
	
	if (ServerReturnTime == HalfRoundTripTime)
	{
		UE_LOG(LogTemp, Log, TEXT("时间同步：往返时间相同：%f"), HalfRoundTripTime);
	}
	else
	{
		const bool bIsHalfLessthanReturn = HalfRoundTripTime < ServerReturnTime;
		FString BoolResult = bIsHalfLessthanReturn ? "true" : "false";

		const float Diff = FMath::Abs(HalfRoundTripTime - ServerReturnTime);
		UE_LOG(LogTemp, Log, TEXT("时间同步：往返时间不同，往返半值小于返回值：%s, 差值：%f"), *BoolResult, Diff);
	}

	const float CurrentServerTime = ReportTimeOfServer + ServerReturnTime;
	
	ClientServerTimeDelta = CurrentServerTime - CurrentClientTimeSeconds;
	ABlasterCharacter::DisplayRole(GetLocalRole());

	const int ClientServerTimeSyncConsoleVariableValue = AConsoleVariableActor::GetClientServerTimeSync();
	if (ClientServerTimeSyncConsoleVariableValue)
	{
		UE_LOG(LogTemp, Log, TEXT("-----------------------"));
		UE_LOG(LogTemp, Log, TEXT("时间同步：客户端请求同步时间：%f"), RequestTimeOfClient);
		UE_LOG(LogTemp, Log, TEXT("时间同步：服务器返回同步时间：%f"), ReportTimeOfServer);
		UE_LOG(LogTemp, Log, TEXT("时间同步：客户端当前时间：%f"), CurrentClientTimeSeconds);
		UE_LOG(LogTemp, Log, TEXT("时间同步：往返时间：%f"), RoundTripTime);
		UE_LOG(LogTemp, Log, TEXT("时间同步：服务器当前时间：%f"), CurrentServerTime);
		UE_LOG(LogTemp, Log, TEXT("客户端与服务器之间的时间差值：%f"), ClientServerTimeDelta);
		UE_LOG(LogTemp, Log, TEXT("-----------------------"));
	}
}