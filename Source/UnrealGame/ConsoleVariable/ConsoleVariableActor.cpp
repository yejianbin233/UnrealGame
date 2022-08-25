// Fill out your copyright notice in the Description page of Project Settings.


#include "ConsoleVariableActor.h"

// Sets default values
AConsoleVariableActor::AConsoleVariableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AConsoleVariableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AConsoleVariableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}








// =========  Custom ConsoleVariable  ===========

static TAutoConsoleVariable<int32> CVarShowLocalRoleDebugInfo(
	TEXT("r.ShowLocalRoleDebugInfo"),
	0,
	TEXT("是否显示 DisplayRole 函数的 Debug 信息")
	TEXT("0：不输出日志信息，默认")
	TEXT("1：输出日志信息"),
	ECVF_Default
);
float AConsoleVariableActor::GetShowLocalRoleDebugInfo()
{
	return CVarShowLocalRoleDebugInfo.GetValueOnGameThread();
}

static TAutoConsoleVariable<int32> CVarClientServerTimeSync(
	TEXT("r.ClientServerTimeSync"),
	0,
	TEXT("客户端服务器时间同步控制台变量，用于控制同步时的日志信息输出")
	TEXT("0：不输出日志信息，默认")
	TEXT("1：输出日志信息"),
	ECVF_Default
);

float AConsoleVariableActor::GetClientServerTimeSync()
{
	return CVarClientServerTimeSync.GetValueOnGameThread();
}

static TAutoConsoleVariable<int32> CVarDetectPickableObjectDebug(
	TEXT("r.DetectPickableObjectDebug"),
	0,
	TEXT("是否输出侦测到的可拾取物品日志信息")
	TEXT("0：不输出日志信息，默认")
	TEXT("1：输出日志信息"),
	ECVF_Default
);
float AConsoleVariableActor::GetDetectPickableObjectDebug()
{
	return CVarDetectPickableObjectDebug.GetValueOnGameThread();
}


static TAutoConsoleVariable<int32> CVarEnterOrLeavePickableItemLog(
	TEXT("r.EnterOrLeavePickableItemLog"),
	0,
	TEXT("是否输出当进入或离开可拾取物品范围的提示日志()")
	TEXT("0：不输出日志信息，默认")
	TEXT("1：输出日志信息"),
	ECVF_Default
);

float AConsoleVariableActor::GetEnterOrLeavePickableItemLog()
{
	return CVarEnterOrLeavePickableItemLog.GetValueOnGameThread();
}

static TAutoConsoleVariable<int32> CVarEnterCameraDebugMode(
	TEXT("u.EnterCameraDebugMode"),
	0,
	TEXT("是否进入摄像机 Debug 模式，可旋转、拉伸摄像机视口")
	TEXT("0：不进入，默认")
	TEXT("1：进入"),
	ECVF_Default
);

float AConsoleVariableActor::GetEnterCameraDebugMode()
{
	return CVarEnterCameraDebugMode.GetValueOnGameThread();
}

static TAutoConsoleVariable<int32> CVarShowCameraDebugLog(
	TEXT("u.ShowCameraDebugLog"),
	0,
	TEXT("是否显示摄像机 Debug 日志")
	TEXT("0：不显式，默认")
	TEXT("1：显示"),
	ECVF_Default
);

float AConsoleVariableActor::GetShowCameraDebugLog()
{
	return CVarShowCameraDebugLog.GetValueOnGameThread();
}



