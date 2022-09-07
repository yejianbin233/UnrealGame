// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemRender.h"

#include "EnhancedInputComponent.h"
#include "Components/SceneCaptureComponent2D.h"

// Sets default values
AItemRender::AItemRender()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootStaticMeshComponent"));
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	Scene2DCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("USceneCaptureComponent2D"));
	ItemStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemStaticMeshComponent"));
	ItemSkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemSkeletalMeshComponent"));
	PointLightComponent = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLightComponent"));

	SetRootComponent(RootStaticMeshComponent);
	ItemStaticMeshComponent->SetupAttachment(RootStaticMeshComponent);
	ItemSkeletalMeshComponent->SetupAttachment(RootStaticMeshComponent);

	SpringArmComponent->SetupAttachment(RootComponent);
	Scene2DCaptureComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	PointLightComponent->SetupAttachment(RootComponent);
}

// TODO 何时调用???
void AItemRender::BindItemRenderInput()
{
	if (!bIsBindInput)
	{
		if (PlayerController && PlayerController->InputComponent)
		{
			if (UEnhancedInputComponent* EInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
			{
				if (EIA_MouseOperatorEnable)
				{
					EInputComponent->BindAction(EIA_MouseOperatorEnable, ETriggerEvent::Triggered, this, &ThisClass::MouseOperatorPressed);
				}

				if (EIA_MouseOperatorDisable)
				{
					EInputComponent->BindAction(EIA_MouseOperatorDisable, ETriggerEvent::Triggered, this, &ThisClass::MouseOperatorReleased);
				}
				
				if (EIA_ClickTap)
				{
					EInputComponent->BindAction(EIA_ClickTap, ETriggerEvent::Triggered, this, &ThisClass::ClickTap);
				}
				
				// if (EIA_YRotator)
				// {
				// 	EInputComponent->BindAction(EIA_YRotator, ETriggerEvent::Triggered, this, &ThisClass::AdjustYOffset);
				// }
				//
				// if (EIA_ZRotator)
				// {
				// 	EInputComponent->BindAction(EIA_ZRotator, ETriggerEvent::Triggered, this, &ThisClass::AdjustZOffset);
				// }

				if (EIA_AdjustArmLength)
				{
					EInputComponent->BindAction(EIA_AdjustArmLength, ETriggerEvent::Triggered, this, &ThisClass::AdjustArmLength);
				}

				if (EIA_ResetRotator)
				{
					EInputComponent->BindAction(EIA_ResetRotator, ETriggerEvent::Triggered, this, &ThisClass::ResetRotator);
				}
			}
		}
		bIsBindInput = true;
	}
}

void AItemRender::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// 设置最大渲染距离，可用于剔除反射的远景物体
	Scene2DCaptureComponent->MaxViewDistanceOverride = 300.0f;

	// Scene2DCaptureComponent 在编辑器中设置"显示标志"，SceneCapture 的 ViewFamily 的 ShowFlags，用于控制此视图的渲染设置。隐藏但可通过详细信息自定义访问

	// 仅显示自身 Actor 内的组件
	Scene2DCaptureComponent->ShowOnlyActorComponents(this);

	DefaultSpringArmLength = SpringArmComponent->TargetArmLength;
}

void AItemRender::SetRenderItemInfo(FItemInfo NewItemInfo)
{
	ItemInfo = NewItemInfo;

	// 设置渲染的网格体(静态/骨骼)
	if (ItemInfo.MeshType == EMeshType::SkeletalMesh)
	{
		ItemSkeletalMeshComponent->SetSkeletalMesh(ItemInfo.SkeletalMesh);
	}
	else if (ItemInfo.MeshType == EMeshType::StaticMesh)
	{
		ItemStaticMeshComponent->SetStaticMesh(ItemInfo.StaticMesh);
	}
}

// Called when the game starts or when spawned
void AItemRender::BeginPlay()
{
	Super::BeginPlay();
	
}

void AItemRender::MouseOperatorPressed()
{
	bItemRenderOperateEnable = true;
}

void AItemRender::MouseOperatorReleased()
{
	bItemRenderOperateEnable = false;
}

void AItemRender::ClickTap()
{
	// 射线检测
	UE_LOG(LogTemp, Log, TEXT("123"));
}

void AItemRender::AdjustArmLength(const FInputActionValue& ActionValue)
{
	float NewArmLength = SpringArmComponent->TargetArmLength;
	NewArmLength += ActionValue.GetMagnitude() * SpringArmZoomSpeed;
	SpringArmComponent->TargetArmLength = FMath::Clamp(NewArmLength, 10, 200);
}

void AItemRender::ResetRotator()
{
	ItemStaticMeshComponent->SetWorldRotation(FRotator(0,0,0));
	ItemSkeletalMeshComponent->SetWorldRotation(FRotator(0,0,0));
}

void AItemRender::ResetArmLength()
{
	SpringArmComponent->TargetArmLength = DefaultSpringArmLength;
}
