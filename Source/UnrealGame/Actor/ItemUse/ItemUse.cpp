// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemUse.h"
#include "UnrealGame/Character/BlasterCharacter.h"
// Sets default values
AItemUse::AItemUse()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AItemUse::Use(ABlasterCharacter* PlayerCharacter)
{
	
}

// Called when the game starts or when spawned
void AItemUse::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AItemUse::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

