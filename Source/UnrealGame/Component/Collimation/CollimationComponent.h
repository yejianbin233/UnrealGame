// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "CollimationComponent.generated.h"

UENUM(BlueprintType)
enum class ECollimationType : uint8
{
	Crosshair UMETA(DisplayName="Crosshair"),
	Circle UMETA(DisplayName="Circle"),
	Snipe UMETA(DisplayName="Snipe")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALGAME_API UCollimationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCollimationComponent();

	virtual void ShowCollimation() PURE_VIRTUAL(UCollimationSceneComponent::ShowCollimation, );

	virtual void HideCollimation() PURE_VIRTUAL(UCollimationSceneComponent::HideCollimation, );

	// 十字准星扩散
	virtual void UpdateSpread() PURE_VIRTUAL(UCollimationSceneComponent::UpdateSpread, );

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	float Spread;

protected:
	UPROPERTY(EditDefaultsOnly, Category="CollimationType")
	ECollimationType CollimationType;

public:
	FORCEINLINE ECollimationType GetCollimationType() const { return CollimationType; };
};
