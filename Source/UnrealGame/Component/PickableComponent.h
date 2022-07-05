// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PickableComponent.generated.h"

UENUM(BlueprintType)
enum class EPickupState : uint8
{
	Pickable UMETA(DisplayName="Pickable"),
	UnPickable UMETA(DisplayName="UnPickable"),
	Picked UMETA(DisplayName="Picked")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALGAME_API UPickableComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPickableComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void UpdatePickupState(EPickupState InPickupState);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
public:	
	
	UPROPERTY(VisibleAnywhere, Category="Weapon Properties")
	class USphereComponent* PickableAreaComponent;

	EPickupState PickupState;

private:
	
	FDelegateHandle OnComponentBeginOverlapDelegateHandle;
	FDelegateHandle OnComponentEndOverlapDelegateHandle;

public:

	FORCEINLINE EPickupState GetPickupState() const { return PickupState; };
};




