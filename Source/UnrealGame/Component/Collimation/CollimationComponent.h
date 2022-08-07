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

// UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
UCLASS(Blueprintable, BlueprintType, meta=(BlueprintSpawnableComponent))
class UNREALGAME_API UCollimationComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadOnly, Category="Reference", DisplayName="玩家角色")
	class ABlasterCharacter* PlayerCharacter;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Widget", DisplayName="准星UI控件类")
	TSubclassOf<class UUserWidget> CrosshairWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category="Widget", DisplayName="准星UI控件")
	class UCrosshairWidget* CrosshairWidget;

protected:
	
	UPROPERTY(EditDefaultsOnly, Category="CollimationType", DisplayName="瞄准准星类型")
	ECollimationType CollimationType;
	
public:	
	// Sets default values for this component's properties
	// UCollimationComponent(const FObjectInitializer& ObjectInitializer);
	UCollimationComponent();

	virtual void Init(ABlasterCharacter* InPlayerCharacter);

	virtual void ShowCollimation() PURE_VIRTUAL(UCollimationSceneComponent::ShowCollimation);

	virtual void HideCollimation() PURE_VIRTUAL(UCollimationSceneComponent::HideCollimation);

protected:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	FORCEINLINE ECollimationType GetCollimationType() const { return CollimationType; };
};
