// Fill out your copyright notice in the Description page of Project Settings.


#include "PickableComponent.h"

#include "Components/SphereComponent.h"
#include "UnrealGame/Character/BlasterCharacter.h"

// Sets default values for this component's properties
UPickableComponent::UPickableComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	PickableAreaComponent = CreateDefaultSubobject<USphereComponent>(TEXT("PickableAreaComponent"));

	PickableAreaComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnComponentBeginOverlap);

	PickableAreaComponent->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnComponentEndOverlap);

	PickableAreaComponent->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	// 默认可拾取
	UpdatePickupState(EPickupState::Pickable);
	// ...
}


// Called when the game starts
void UPickableComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
void UPickableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// ...
}

void UPickableComponent::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if (ensure(BlasterCharacter))
	{
		BlasterCharacter->bHasPickableObject = true;
	}
}

void UPickableComponent::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if (ensure(BlasterCharacter))
	{
		BlasterCharacter->bHasPickableObject = false;
	}
}

void UPickableComponent::UpdatePickupState(EPickupState InPickupState)
{
	PickupState = InPickupState;
	if (InPickupState == EPickupState::Pickable)
	{
		PickableAreaComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		PickableAreaComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

		PickableAreaComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	} else
	{
		PickableAreaComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		PickableAreaComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	}

	PickableAreaComponent->SetSphereRadius(Radius);
}




