// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Casing.generated.h"

// 子弹壳
UCLASS()
class UNREALGAME_API ACasing : public AActor
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Component", DisplayName="子弹壳网格体")
	UStaticMeshComponent* CasingMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Casing", DisplayName="子弹壳冲击力")
	float ShellEjectionImpulse;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Casing", DisplayName="子弹壳声音效果")
	class USoundCue* ShellSound;
	
public:	
	// Sets default values for this actor's properties
	ACasing();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent
			, AActor* OtherActor
			, UPrimitiveComponent* OtherComponent
			, FVector NormalImpulse
			, const FHitResult& HitResult
		);
	
};
