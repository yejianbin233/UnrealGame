// Fill out your copyright notice in the Description page of Project Settings.


#include "Unreal2DTileMapActor.h"
#include "PaperTileMapComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "UnrealGame/Camera/Unreal2DCameraActor.h"
#include "UnrealGame/Character/Unreal2DCharacter.h"

AUnreal2DTileMapActor::AUnreal2DTileMapActor()
{
	LeftMapCameraBlockBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftMapCameraBlockBoxComponent"));
	
	RightMapCameraBlockBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("RightMapCameraBlockBoxComponent"));
	
	TileMapLeftTopPoint = CreateDefaultSubobject<UBoxComponent>(TEXT("TileMapLeftTopPoint"));
	TileMapLeftTopPoint->SetBoxExtent(FVector(0.1f, 0.1f, 0.1f));
	TileMapLeftTopPoint->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	LeftMapCameraBlockBoxComponent->SetupAttachment(RootComponent);
	RightMapCameraBlockBoxComponent->SetupAttachment(RootComponent);
	TileMapLeftTopPoint->SetupAttachment(RootComponent);

	CameraChildComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("CameraChildComponent"));
	CameraChildComponent->SetupAttachment(RootComponent);
}

void AUnreal2DTileMapActor::BeginPlay()
{
	Super::BeginPlay();
}

void AUnreal2DTileMapActor::Init()
{
	int32 MapWidth;
	int32 MapHeight;
	int32 MapLayerNum;
	GetRenderComponent()->GetMapSize(MapWidth, MapHeight, MapLayerNum);

	FVector2D TileSpriteCell;
	FVector LeftTopLocation = TileMapLeftTopPoint->GetRelativeLocation();
	LeftTopLocation.Y = 0;

	OriginPoint = LeftTopLocation;

	TileSpriteCell.X = FMath::Abs(LeftTopLocation.X);
	TileSpriteCell.Y = FMath::Abs(LeftTopLocation.Z);
	TileSetCellWidthHeight = TileSpriteCell;
	
	float TileMapWidth = MapWidth * TileSpriteCell.X;
	float TileMapHeight = MapHeight * TileSpriteCell.Y;
	
	TileMapWidthHeight.X = TileMapWidth + TileMapWidth; 
	TileMapWidthHeight.Y = TileMapHeight + TileMapHeight;

	TileMapCenterPoint.X = (LeftTopLocation.X + TileMapWidthHeight.X) / 2;
	TileMapCenterPoint.Y = (LeftTopLocation.Y - TileMapWidthHeight.Y) / 2;

	LeftMapCameraBlockBoxComponent->SetBoxExtent(FVector(BlockCameraBoxWidth
		, BlockCameraBoxThickness
		, TileMapHeight));

	FVector LeftBoxLocation;
	LeftBoxLocation.X = LeftTopLocation.X + BlockCameraBoxWidth;
	LeftBoxLocation.Y = 0;
	LeftBoxLocation.Z = -TileMapHeight + LeftTopLocation.Z;

	CameraLeftBlockXPosition = GetActorLocation().X + LeftBoxLocation.X + BlockCameraBoxWidth;
	LeftMapCameraBlockBoxComponent->SetRelativeLocation(LeftBoxLocation);

	RightMapCameraBlockBoxComponent->SetBoxExtent(FVector(BlockCameraBoxWidth
		, BlockCameraBoxThickness
		, TileMapHeight));

	FVector RightBoxLocation;
	RightBoxLocation.X = LeftTopLocation.X + (TileMapWidth * 2) - BlockCameraBoxWidth;
	RightBoxLocation.Y = 0;
	RightBoxLocation.Z = -TileMapHeight + LeftTopLocation.Z;

	CameraRightBlockXPosition = GetActorLocation().X + RightBoxLocation.X - BlockCameraBoxWidth;
	
	RightMapCameraBlockBoxComponent->SetRelativeLocation(RightBoxLocation);
}

void AUnreal2DTileMapActor::CameraBlockBoxInit()
{
	if (bEnableCameraLeftBlock)
	{
		LeftMapCameraBlockBoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	else
	{
		LeftMapCameraBlockBoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (bEnableCameraRightBlock)
	{
		RightMapCameraBlockBoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	else
	{
		RightMapCameraBlockBoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	LeftMapCameraBlockBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AUnreal2DTileMapActor::OnLeftMapCameraBlockBoxComponentBeginOverlap);

	LeftMapCameraBlockBoxComponent->OnComponentEndOverlap.AddDynamic(this, &AUnreal2DTileMapActor::OnLeftMapCameraBlockBoxComponentEndOverlap);

	RightMapCameraBlockBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AUnreal2DTileMapActor::OnRightMapCameraBlockBoxComponentBeginOverlap);

	RightMapCameraBlockBoxComponent->OnComponentEndOverlap.AddDynamic(this, &AUnreal2DTileMapActor::OnRightMapCameraBlockBoxComponentEndOverlap);
}

void AUnreal2DTileMapActor::OnLeftMapCameraBlockBoxComponentBeginOverlap(UPrimitiveComponent* PrimitiveComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	AUnreal2DCharacter* Player = Cast<AUnreal2DCharacter>(OtherActor);

	if (Player)
	{
		if(ensureMsgf(Player->CameraActor, TEXT("玩家角色的摄像机Actor为 nullptr")))
		{
			Player->CameraActor->BlockHandle(CameraLeftBlockXPosition);
		}
	}
}

void AUnreal2DTileMapActor::OnRightMapCameraBlockBoxComponentBeginOverlap(UPrimitiveComponent* PrimitiveComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	AUnreal2DCharacter* Player = Cast<AUnreal2DCharacter>(OtherActor);

	if (Player)
	{
		if(ensureMsgf(Player->CameraActor, TEXT("玩家角色的摄像机Actor为 nullptr")))
		{
			Player->CameraActor->BlockHandle(CameraRightBlockXPosition);
		}
	}
}

void AUnreal2DTileMapActor::OnLeftMapCameraBlockBoxComponentEndOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AUnreal2DCharacter* Player = Cast<AUnreal2DCharacter>(OtherActor);

	if (Player)
	{
		Player->CameraActor->CancleBlockHandle(CameraLeftBlockXPosition);
	}
}

void AUnreal2DTileMapActor::OnRightMapCameraBlockBoxComponentEndOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AUnreal2DCharacter* Player = Cast<AUnreal2DCharacter>(OtherActor);

	if (Player)
	{
		Player->CameraActor->CancleBlockHandle(CameraRightBlockXPosition);
	}
}

