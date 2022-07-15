// Fill out your copyright notice in the Description page of Project Settings.


#include "Unreal2DCameraActor.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "UnrealGame/Character/Unreal2DCharacter.h"

// Sets default values
AUnreal2DCameraActor::AUnreal2DCameraActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));

	SetRootComponent(SpringArmComponent);
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
}

// Called when the game starts or when spawned
void AUnreal2DCameraActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AUnreal2DCameraActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	UpdateCameraFollowPlayer(DeltaTime);
}

void AUnreal2DCameraActor::SetCameraOwner(AUnreal2DCharacter* InPlayerCharacter)
{
	PlayerCharacter = InPlayerCharacter;
}

void AUnreal2DCameraActor::UpdateCameraFollowPlayer(float DeltaTime)
{
	if (PlayerCharacter)
	{
		if (bIsBlocking)
		{
			BlockBlendTime += DeltaTime;

			if (ensureMsgf(CameraBlendCurve, TEXT("摄像机曲线资源未设置")))
			{
				FVector CameraLocation = GetActorLocation();
				FVector PlayerLocation = PlayerCharacter->GetActorLocation();
				FVector NewCameraLocation;
				
				// 当阻塞时，摄像机的 X 轴不变，XZ 跟随玩家的位置变化
				NewCameraLocation.X = UKismetMathLibrary::Lerp(CameraLocation.X, CameraBlockXLocation, CameraBlendCurve->GetFloatValue(BlockBlendTime));
				NewCameraLocation.Y = PlayerLocation.Y;
				NewCameraLocation.Z = PlayerLocation.Z;
				SetActorLocation(NewCameraLocation);
			}
		}
		else
		{
			// 不阻塞时，时刻跟随玩家角色
			SetActorLocation(PlayerCharacter->GetActorLocation());
		}
	}
}

void AUnreal2DCameraActor::BlockHandle(float BlockXLocation)
{
	if (!bIsBlocking)
	{
		// 第一次阻塞
		bIsBlocking = true;
	}
	else
	{
		// 重复阻塞，则说明是"转场"，即从一个地图块进入另一个地图块

		FVector CameraLocation = GetActorLocation();
			
		FVector NewCameraLocation;
		
		// 直接切换到另一个地图块对应的阻塞位置
		NewCameraLocation.X = BlockXLocation;
		NewCameraLocation.Y = CameraLocation.Y;
		NewCameraLocation.Z = CameraLocation.Z;
		SetActorLocation(NewCameraLocation);
	}

	// 相同的 X 轴值没有差别，重复不添加
	CameraBlockXValues.AddUnique(BlockXLocation);
	CameraBlockXLocation = BlockXLocation;
}

void AUnreal2DCameraActor::CancleBlockHandle(float CancleBlockXValue)
{

	bool bIsToggle = CancleBlockXValue == CameraBlockXLocation;
	
	for (int I=0; I < CameraBlockXValues.Num(); I++)
	{
		if (CameraBlockXValues[I] == CancleBlockXValue)
		{
			CameraBlockXValues.RemoveAt(I);
		}
	}
	
	if (CameraBlockXValues.Num() == 0)
	{
		bIsBlocking = false;
	}
	else if(CameraBlockXValues.Num() > 0 && bIsToggle)
	{
		// 一般来说数组内最多同时存在两个元素，移除一个还剩下一个，直接使用下标"0"。
		CameraBlockXLocation = CameraBlockXValues[0];
	}
}
