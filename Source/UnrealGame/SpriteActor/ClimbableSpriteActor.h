// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperSpriteActor.h"
#include "ClimbableSpriteActor.generated.h"

UENUM(BlueprintType)
enum class EClimbableType : uint8
{
	Wall UMETA(DisplayName="墙壁"),
	Vine UMETA(DisplayName="藤蔓"),
	Ladder UMETA(DisplayName="梯子"),
};

UCLASS()
class UNREALGAME_API AClimbableSpriteActor : public APaperSpriteActor
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Component", DisplayName="可攀爬检测区域")
	class UBoxComponent* ClimbableAreaComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Component", DisplayName="攀爬阻挡区域")
	class UBoxComponent* ClimbableBlockAreaComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Component", DisplayName="可攀爬起点目标点")
	class UChildActorComponent* ClimbableStartComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Component", DisplayName="可攀爬终点目标点")
	class UChildActorComponent* ClimbableEndComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Type", DisplayName="可攀爬区域类型")
	EClimbableType ClimbableType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Location", DisplayName="可攀爬起点位置")
	FVector ClimbStartLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Location", DisplayName="可攀爬终点位置")
	FVector ClimbEndLocation;
	
public:	
	// Sets default values for this actor's properties
	AClimbableSpriteActor();

	/*
	 * @description: Init- 根据 Actor 在场景中的数据，计算可攀爬物体的攀爬起点和攀爬终点
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月20日 星期三 15:07:20
	 */
	void Init();

	/*
	 * @description: CalClimbPosition 根据玩家角色当前位置计算攀爬位置
	 * @param PlayerLocation - 玩家位置
	 * @Param ClimbValue - 攀爬量
	 * @Param ToUp - true 往上攀爬；false 往下攀爬
	 * 
	 * @return FVector 攀爬移动位置
	 * ...
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月20日 星期三 15:07:18
	 */
	FVector CalClimbPosition(FVector PlayerLocation, float ClimbValue, bool ToUp);

	/*
	 * @description:GetRecentClimbLocation - 根据玩家位置获取攀爬actor上最近的一个可攀爬点
	 * @param PlayerLocation - 角色玩家位置
	 * @return FVector - 最近可攀爬点
	 * ...
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月20日 星期三 15:07:01
	 */
	FVector GetRecentClimbLocation(FVector PlayerLocation);

	/*
	 * @description: IsToEnd - 盘点玩家是否攀爬到目标终点（向上攀爬终点/向下攀爬终点）
	 * @param ZValue - Z 轴
	 * @param ToUp - true 向上；false 向下 
	 * 
	 * @return bool - 结果
	 * ...
	 * 
	 * @author: yejianbin
	 * @version: v1.0
	 * @createTime: 2022年07月20日 星期三 15:07:45
	 */
	bool IsToEnd(float ZValue, bool ToUp);

	UFUNCTION(Server, Reliable, Category="Collision", DisplayName="当可攀爬检测区域开始重叠时")
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(Server, Reliable, Category="Collision", DisplayName="当可攀爬检测区域结束重叠时")
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
