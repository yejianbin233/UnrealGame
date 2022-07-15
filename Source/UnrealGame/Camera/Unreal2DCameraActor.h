// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Unreal2DCameraActor.generated.h"

/*
 * 摄像机 Actor
 */
UCLASS()
class UNREALGAME_API AUnreal2DCameraActor : public AActor
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera", DisplayName="摄像机弹簧臂组件")
	class USpringArmComponent* SpringArmComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera", DisplayName="摄像机组件")
	class UCameraComponent* CameraComponent;

	UPROPERTY(BlueprintReadWrite, Category="Reference", DisplayName="玩家角色")
	class AUnreal2DCharacter* PlayerCharacter;

	UPROPERTY(BlueprintReadWrite, Category="Camera", DisplayName="摄像机是否处于阻塞中状态")
	bool bIsBlocking;

	UPROPERTY(BlueprintReadWrite, Category="Camera", DisplayName="摄像机阻塞次数")
	float OverlapNum;

	UPROPERTY(BlueprintReadWrite, Category="Camera", DisplayName="摄像机阻塞X轴位置")
	float CameraBlockXLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera", DisplayName="摄像机混合曲线")
	UCurveFloat* CameraBlendCurve;

	/*
	 * NOTE：可以在 蓝图中使用 "TimeLineComponent(时间轴组件)"来实现，而不需要定义"BlockBlendTime"，定义"BlockBlendTime"只不过是使用多种方式处理而已
	 */
	UPROPERTY(Category="Camera", DisplayName="阻塞混合摄像机的时间")
	float BlockBlendTime;

	UPROPERTY(BlueprintReadWrite, Category="Camera", DisplayName="阻塞X轴位置数组")
	TArray<float> CameraBlockXValues;
	
public:	
	// Sets default values for this actor's properties
	AUnreal2DCameraActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/*
	 * @description: SetCameraOwner - 设置摄像机的玩家角色拥有者
	 * @param InPlayerCharacter - 玩家角色
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月15日 星期五 18:07:11
	 */
	UFUNCTION(BlueprintCallable, Category="Camera", DisplayName="设置摄像机的玩家角色拥有者")
	void SetCameraOwner(AUnreal2DCharacter* InPlayerCharacter);

	/*
	 * @description: UpdateCameraFollowPlayer - 摄像机跟随玩家角色，并且与"Unreal2DTileMapActor"交互，影响摄像机的移动
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月15日 星期五 18:07:52
	 */
	void UpdateCameraFollowPlayer(float DeltaTime);

	/*
	 * @description: BlockHandle - 与"Unreal2DTileMapActor"交互时，阻挡摄像机时调用
	 * @param BlockXLocation - 阻挡时仅影响 X 轴，YZ 跟随玩家角色的位置
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月15日 星期五 18:07:44
	 */
	virtual void BlockHandle(float BlockXLocation);

	/*
	 * @description: BlockHandle - 与"Unreal2DTileMapActor"交互时，取消阻挡摄像机时调用
	 * @param CancleBlockXValue - 曲线阻挡的 X 轴位置值
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月15日 星期五 18:07:44
	 */
	virtual void CancleBlockHandle(float CancleBlockXValue);
};
