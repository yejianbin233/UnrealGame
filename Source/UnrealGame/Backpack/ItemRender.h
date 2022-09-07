// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"
#include "ItemInfoObject.h"
#include "Components/PointLightComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/SpringArmComponent.h"
#include "ItemRender.generated.h"

UCLASS(BlueprintType, Blueprintable)
class UNREALGAME_API AItemRender : public AActor
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Item Render", DisplayName="根静态网格体组件")
	UStaticMeshComponent* RootStaticMeshComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Item Render", DisplayName="物品展示弹簧臂")
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Item Render", DisplayName="场景2D捕获组件")
	USceneCaptureComponent2D* Scene2DCaptureComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Item Render", DisplayName="物体静态网格体组件")
	UStaticMeshComponent* ItemStaticMeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Item Render", DisplayName="物体骨骼网格体组件")
	USkeletalMeshComponent* ItemSkeletalMeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Item Render", DisplayName="纹理渲染目标")
	UTextureRenderTarget2D* TextureRenderTarget2D;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Item Render", DisplayName="点光源组件")
	UPointLightComponent* PointLightComponent;

	UPROPERTY(BlueprintReadWrite, Category="Reference", DisplayName="玩家控制器")
	APlayerController* PlayerController;
	
	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputMappingContext")
	TObjectPtr<class UInputMappingContext> ItemRenderInputMappingContext;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="启用鼠标操作")
	TObjectPtr<class UInputAction> EIA_MouseOperatorEnable;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="禁止鼠标操作")
	TObjectPtr<class UInputAction> EIA_MouseOperatorDisable;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="轻敲(点击)")
	TObjectPtr<class UInputAction> EIA_ClickTap;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="调整弹簧臂长度")
	TObjectPtr<class UInputAction> EIA_AdjustArmLength;

	UPROPERTY(EditDefaultsOnly, Category="EnhancedInput | InputAction", DisplayName="调整弹簧臂长度")
	TObjectPtr<class UInputAction> EIA_ResetRotator;

private:

	UPROPERTY(BlueprintReadWrite, Category="Item Render", DisplayName="是否已绑定输入", meta=(AllowPrivateAccess))
	FItemInfo ItemInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player Input", DisplayName="Y轴旋转速率", meta=(AllowPrivateAccess))
	float YRotateSpeed=1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player Input", DisplayName="Z轴旋转速率", meta=(AllowPrivateAccess))
	float ZRotateSpeed=1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player Input", DisplayName="弹簧臂缩放速率", meta=(AllowPrivateAccess))
	float SpringArmZoomSpeed=1.0f;
	
	UPROPERTY(BlueprintReadWrite, Category="Player Input", DisplayName="是否已绑定输入", meta=(AllowPrivateAccess))
	bool bIsBindInput;

	UPROPERTY(BlueprintReadWrite, Category="Player Input", DisplayName="是否启用YZ轴旋转操作", meta=(AllowPrivateAccess))
	bool bItemRenderOperateEnable;

	UPROPERTY(BlueprintReadWrite, Category="Spring", DisplayName="弹簧臂默认长度", meta=(AllowPrivateAccess))
	float DefaultSpringArmLength;

public:	
	// Sets default values for this actor's properties
	AItemRender();

	virtual void BindItemRenderInput();
	
	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION(BlueprintCallable, Category="Player Input", DisplayName="启用物品渲染操作")
	void MouseOperatorPressed();

	UFUNCTION(BlueprintCallable, Category="Player Input", DisplayName="禁止物品渲染操作")
	void MouseOperatorReleased();

	// TODO
	UFUNCTION(BlueprintCallable, Category="Player Input", DisplayName="点击")
	void ClickTap();

	UFUNCTION(BlueprintCallable, Category="Player Input", DisplayName="调整弹簧臂长度")
	void AdjustArmLength(const FInputActionValue& ActionValue);

	UFUNCTION(BlueprintCallable, Category="Player Input", DisplayName="重置物体旋转")
	void ResetRotator();

	UFUNCTION(BlueprintCallable, Category="Spring", DisplayName="重置弹簧臂长度")
	void ResetArmLength();
	/*
	 * 可拓展 -
	 *
	 * 定义一个结构体，用于设置根据不同大小或特殊需求的网格体渲染显示，如调整弹簧臂长度，调整相机属性，调整灯光属性等
	 *
	 * 根据不同渲染物品，定义一个配置属性集。
	 */
	UFUNCTION(BlueprintCallable, Category="Item Render", DisplayName="设置渲染物品数据")
	virtual void SetRenderItemInfo(FItemInfo NewItemInfo);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

};
