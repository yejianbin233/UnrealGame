// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperTileMapActor.h"
#include "UnrealGame/Character/Unreal2DCharacter.h"
#include "Unreal2DTileMapActor.generated.h"

/**
 * AUnreal2DTileMapActor - 地图块 Actor
 */
UCLASS()
class UNREALGAME_API AUnreal2DTileMapActor : public APaperTileMapActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Component", DisplayName="左边阻挡摄像机碰撞盒子组件")
	class UBoxComponent* LeftMapCameraBlockBoxComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Component", DisplayName="右边阻挡摄像机碰撞盒子组件")
	class UBoxComponent* RightMapCameraBlockBoxComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Component", DisplayName="图块地图左上角位置(手动放置到原点)")
	class UBoxComponent* TileMapLeftTopPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera", DisplayName="阻挡摄像机盒子的宽度")
	float BlockCameraBoxWidth = 200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera", DisplayName="阻挡摄像机盒子的厚度")
	float BlockCameraBoxThickness = 100;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Camera", DisplayName="地图摄像机")
	class UCameraComponent* MapCameraComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Camera", DisplayName="地图摄像机")
	UChildActorComponent* CameraChildComponent;
	
	UPROPERTY(BlueprintReadWrite, Category="TileMap", DisplayName="地图块原点")
	FVector OriginPoint;
	UPROPERTY(BlueprintReadWrite, Category="TileMap", DisplayName="地图块集单元格长宽")
	FVector2D TileSetCellWidthHeight;
	UPROPERTY(BlueprintReadWrite, Category="TileMap", DisplayName="地图块总体长宽")
	FVector2D TileMapWidthHeight;
	UPROPERTY(BlueprintReadWrite, Category="TileMap", DisplayName="地图块中心点")
	FVector2D TileMapCenterPoint;

	UPROPERTY(BlueprintReadWrite, Category="Camera", DisplayName="摄像机左边阻挡的X值")
	float CameraLeftBlockXPosition;
	UPROPERTY(BlueprintReadWrite, Category="Camera", DisplayName="摄像机右边阻挡的X值")
	float CameraRightBlockXPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera", DisplayName="是否启用地图块左边摄像机阻塞")
	bool bEnableCameraLeftBlock = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera", DisplayName="是否启用地图块右边摄像机阻塞")
	bool bEnableCameraRightBlock = false;
	
public:

	AUnreal2DTileMapActor();

	virtual void BeginPlay() override;

	/*
	 * @description: Init - 初始化地图块中的各种数据，如：地图两边的摄像机阻塞盒子，地图块的属性数据(地图块中心点、地图块的大小)
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月15日 星期五 18:07:47
	 */
	UFUNCTION(BlueprintCallable, Category="Camera", DisplayName="初始化")
	void Init();

	/*
	 * @description: CameraBlockBoxInit - 被蓝图构造函数所调用，初始化地图两边的摄像机阻塞盒子的碰撞设置
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月15日 星期五 18:07:29
	 */
	UFUNCTION(BlueprintCallable, Category="Camera", DisplayName="初始化摄像机碰撞盒子")
	void CameraBlockBoxInit();

	UFUNCTION()
	void OnLeftMapCameraBlockBoxComponentBeginOverlap(UPrimitiveComponent* PrimitiveComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnLeftMapCameraBlockBoxComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnRightMapCameraBlockBoxComponentBeginOverlap(UPrimitiveComponent* PrimitiveComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnRightMapCameraBlockBoxComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/*
	 * @description: SwitchViewTarget - 切换摄像机视角，玩家摄像机 <==> 地图块摄像机
	 * @param PlayerCharacter - 切换视角的玩家角色
	 * @param bToMapViewTarget - true - 切换到地图块摄像机， false - 切换到玩家角色摄像机
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月15日 星期五 18:07:24
	 */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Camera", DisplayName="切换地图块视口")
	void SwitchViewTarget(AUnreal2DCharacter* PlayerCharacter, bool bToMapViewTarget);
	
};
