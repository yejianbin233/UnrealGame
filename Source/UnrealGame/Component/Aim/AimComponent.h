// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "AimComponent.generated.h"


UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALGAME_API UAimComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category="Reference", DisplayName="玩家角色")
	class ABlasterCharacter* PlayerCharacter;

	UPROPERTY(BlueprintReadOnly, Category="Reference", DisplayName="玩家角色控制器")
	class ABlasterPlayerController* PlayerController;

	FTimeline AimStartedInterpTimelineComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Aim", DisplayName="结束瞄准插值时间轴组件")
	class UTimelineComponent* AimEndedInterpTimelineComponent;

	UPROPERTY(VisibleAnywhere, Category="Aim", DisplayName="默认视野")
	float DefaultFOV;

	UPROPERTY(EditDefaultsOnly, Category="Aim", DisplayName="瞄准视野")
	float AimFOV;
	
	UPROPERTY(BlueprintReadOnly, Category="Aim", DisplayName="是否正在瞄准中")
	bool bIsAiming;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aim", DisplayName="开始瞄准插值曲线")
	class UCurveFloat* AimStartedInterpCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aim", DisplayName="结束瞄准插值曲线")
	class UCurveFloat* AimEndedInterpCurve;
	
	FOnTimelineFloat OnAimStarted;
	
	FOnTimelineFloat OnAimEnded;
	
public:	
	// Sets default values for this component's properties
	UAimComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category="Aim", DisplayName="瞄准")
	void Aiming(bool bToAim);
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category="Aim", DisplayName="开始瞄准插值")
	void AimStartedFOVInterp(float InterpValue);

	UFUNCTION(BlueprintCallable, Category="Aim", DisplayName="结束瞄准插值")
	void AimEndedFOVInterp(float InterpValue);

	UFUNCTION(BlueprintCallable, Category="Aim", DisplayName="摄像机瞄准")
	void CameraAim(bool bToAim);

	// 瞄准的控件处理
	virtual void WidgetAim(bool bToAim) PURE_VIRTUAL(UAimComponent::CameraAim);
};
