// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealGameplayStatics.h"

// Sets default values
AUnrealGameplayStatics::AUnrealGameplayStatics()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AUnrealGameplayStatics::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AUnrealGameplayStatics::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

