// Fill out your copyright notice in the Description page of Project Settings.


#include "Unreal2DPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "UnrealGame/Camera/Unreal2DCameraActor.h"
#include "UnrealGame/Character/Unreal2DCharacter.h"
#include "UnrealGame/GameMode/Unreal2DGameModeBase.h"

void AUnreal2DPlayerController::CreatePlayerCharacter_Implementation(TSubclassOf<AUnreal2DCharacter> PlayerCharacterClass, FVector InSpawnLocation)
{
	FTransform ActorSpawnTransform;
	ActorSpawnTransform.SetLocation(InSpawnLocation);

	FActorSpawnParameters ActorSpawnParameters;
	ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// 不知道原因：生成位置与实际位置不同，临时解决方式：直接设置
	AUnreal2DCharacter* PlayerCharacter = Cast<AUnreal2DCharacter>(GetWorld()->SpawnActor(PlayerCharacterClass, &ActorSpawnTransform, ActorSpawnParameters));

	UE_LOG(LogTemp, Warning, TEXT("Player Spawn Location X%f - Y%f - Z%f"), ActorSpawnTransform.GetLocation().X, ActorSpawnTransform.GetLocation().Y, ActorSpawnTransform.GetLocation().Z);
	UE_LOG(LogTemp, Warning, TEXT("Player Location X%f - Y%f - Z%f"), PlayerCharacter->GetActorLocation().X, PlayerCharacter->GetActorLocation().Y, PlayerCharacter->GetActorLocation().Z);
	PlayerCharacter->SetActorLocation(InSpawnLocation);
	this->Possess(PlayerCharacter);
	CreatePlayerCamera();
}

void AUnreal2DPlayerController::InitializedWidget_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Init Widget"));
	SelectCharacterWidget = CreateWidget(this, SelectCharacterWidgetClass, FName("SelectCharacterWidget"));

	SelectCharacterWidget->AddToViewport();
}



void AUnreal2DPlayerController::RequestSpawnPlayerCharacter_Implementation(
	TSubclassOf<AUnreal2DCharacter> PlayerCharacterClass)
{
	AUnreal2DGameModeBase* GameMode = Cast<AUnreal2DGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));

	if (GameMode)
	{
		UE_LOG(LogTemp, Warning, TEXT("Controller Request Spawn"));
		GameMode->RequestSpawnPlayerCharacter(PlayerCharacterClass, this);
	}
}

void AUnreal2DPlayerController::BeginPlay()
{
	Super::BeginPlay();

	InitializedWidget();
}

void AUnreal2DPlayerController::CreatePlayerCamera()
{
	UE_LOG(LogTemp, Warning, TEXT("Create Player Camera"));
	AUnreal2DCharacter* PlayerCharacter = Cast<AUnreal2DCharacter>(GetPawn());
	if (PlayerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("Create Camera"));
		FTransform ActorCameraSpawnTransform;
		FVector CameraSpawnLocation = PlayerCharacter->GetActorLocation();
		FRotator CameraSpawnRotator(0, -90, 0);
		ActorCameraSpawnTransform.SetLocation(CameraSpawnLocation);
		ActorCameraSpawnTransform.SetRotation(CameraSpawnRotator.Quaternion());
	
		FActorSpawnParameters ActorSpawnParameters;
		ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
		AUnreal2DCameraActor* CameraActor = Cast<AUnreal2DCameraActor>(GetWorld()->SpawnActor(PlayerCameraClass, &ActorCameraSpawnTransform, ActorSpawnParameters));

		PlayerCharacter->SetCameraActor(CameraActor);
		CameraActor->SetCameraOwner(PlayerCharacter);
		CameraActor->SetReplicates(true);
	}
}
