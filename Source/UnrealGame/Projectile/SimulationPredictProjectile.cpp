// Fill out your copyright notice in the Description page of Project Settings.


#include "SimulationPredictProjectile.h"

#include "Components/BoxComponent.h"

// Sets default values
ASimulationPredictProjectile::ASimulationPredictProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
}

void ASimulationPredictProjectile::Simulation(float SimulationTime, FVector NewCollisionExtent)
{
	GetWorld()->GetTimerManager().SetTimer(PredictProjectileTimerHandle, this, &ThisClass::SimulationEnded, SimulationTime,false);
	CollisionBox->SetBoxExtent(NewCollisionExtent);
}

// Called when the game starts or when spawned
void ASimulationPredictProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASimulationPredictProjectile::SimulationEnded()
{
	Destroy();
}

// Called every frame
void ASimulationPredictProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

