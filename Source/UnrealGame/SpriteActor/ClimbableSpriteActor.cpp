// Fill out your copyright notice in the Description page of Project Settings.


#include "ClimbableSpriteActor.h"
#include "PaperSpriteComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "UnrealGame/Character/Unreal2DCharacter.h"
#include "UnrealGame/Component/2D/Ureal2DClimbableComponent.h"

// Sets default values
AClimbableSpriteActor::AClimbableSpriteActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ClimbableAreaComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("ClimbableAreaComponent"));
	ClimbableBlockAreaComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("ClimbableBlockAreaComponent"));
	ClimbableStartComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("ClimbableStartComponent"));
	ClimbableEndComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("ClimbableEndComponent"));
	
	ClimbableAreaComponent->SetupAttachment(GetRootComponent());
	ClimbableBlockAreaComponent->SetupAttachment(GetRootComponent());
	ClimbableStartComponent->SetupAttachment(GetRootComponent());
	ClimbableEndComponent->SetupAttachment(GetRootComponent());

	ClimbableAreaComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnComponentBeginOverlap);
	ClimbableAreaComponent->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnComponentEndOverlap);
}

// Called every frame
void AClimbableSpriteActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called when the game starts or when spawned
void AClimbableSpriteActor::BeginPlay()
{
	Super::BeginPlay();

	Init();
}

void AClimbableSpriteActor::Init()
{
	ClimbStartLocation = ClimbableStartComponent->GetChildActor()->GetActorLocation();
	
	ClimbEndLocation = ClimbableEndComponent->GetChildActor()->GetActorLocation();

	// DrawDebugSphere(GetWorld(), ClimbStartLocation, 20, 10, FColor::Red, false, 100);
	// DrawDebugSphere(GetWorld(), ClimbEndLocation, 20, 10, FColor::Red, false, 100);
}

FVector AClimbableSpriteActor::CalClimbPosition(FVector PlayerLocation, float ClimbValue, bool ToUp)
{
	const float X = ClimbStartLocation.X;
	const float Y = PlayerLocation.Y;
	
	const float ZValue = ToUp ? PlayerLocation.Z + ClimbValue : PlayerLocation.Z - ClimbValue;
	
	const float Z = FMath::Clamp(ZValue, ClimbStartLocation.Z, ClimbEndLocation.Z);

	UE_LOG(LogTemp, Warning, TEXT("return %f"), Z);
	return FVector(X, Y, Z);
}

FVector AClimbableSpriteActor::GetRecentClimbLocation(FVector PlayerLocation)
{
	FVector RecentClimbLocation;
	RecentClimbLocation.X = FMath::Clamp<float>(PlayerLocation.X, ClimbStartLocation.X, ClimbEndLocation.X);
	RecentClimbLocation.Y = PlayerLocation.Y;
	RecentClimbLocation.Z = FMath::Clamp<float>(PlayerLocation.Z, ClimbStartLocation.Z, ClimbEndLocation.Z);
	
	return RecentClimbLocation;
}

bool AClimbableSpriteActor::IsToEnd(float ZValue, bool ToUp)
{
	return ToUp ? ZValue == ClimbEndLocation.Z : ZValue == ClimbStartLocation.Z;
}

void AClimbableSpriteActor::OnComponentBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComponent,
                                                                   AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                                                   const FHitResult& SweepResult)
{
	AUnreal2DCharacter* PlayerCharacter = Cast<AUnreal2DCharacter>(OtherActor);

	if (PlayerCharacter)
	{
		PlayerCharacter->GetClimbComponent()->EnterOrLeaveClimbableArea(this, true);
	}
}

void AClimbableSpriteActor::OnComponentEndOverlap_Implementation(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AUnreal2DCharacter* PlayerCharacter = Cast<AUnreal2DCharacter>(OtherActor);

	if (PlayerCharacter)
	{
		PlayerCharacter->GetClimbComponent()->EnterOrLeaveClimbableArea(this, false);
	}
}

