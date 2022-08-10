// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CollimationComponent.h"
#include "Curves/CurveVector.h"
#include "CrosshairComponent.generated.h"

USTRUCT(BlueprintType)
struct FCrosshairHUDPackage
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, DisplayName="准心中心")
	class UTexture2D* CrosshairsCenter;

	UPROPERTY(BlueprintReadWrite, DisplayName="准心顶部")
	class UTexture2D* CrosshairsTop;
	
	UPROPERTY(BlueprintReadWrite, DisplayName="准心左边")
	class UTexture2D* CrosshairsLeft;

	UPROPERTY(BlueprintReadWrite, DisplayName="准心底部")
	class UTexture2D* CrosshairsBottom;

	UPROPERTY(BlueprintReadWrite, DisplayName="准心右边")
	class UTexture2D* CrosshairsRight;

	UPROPERTY(BlueprintReadWrite, DisplayName="准心扩大量")
	float CrosshairSpread;

	UPROPERTY(BlueprintReadWrite, DisplayName="准心颜色值")
	FLinearColor CrosshairsColor;
};
/**
 * 
 */
UCLASS()
class UNREALGAME_API UCrosshairComponent : public UCollimationComponent
{
	GENERATED_BODY()

public:
	// 准心由 5 个部分组成
	// 纹理压缩设置"用户界面2D"
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "UI | Crosshairs", DisplayName="准心中心")
	class UTexture2D* CrosshairsCenter;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "UI | Crosshairs", DisplayName="准心顶部")
	class UTexture2D* CrosshairsTop;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "UI | Crosshairs", DisplayName="准心左边")
	class UTexture2D* CrosshairsLeft;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "UI | Crosshairs", DisplayName="准心底部")
	class UTexture2D* CrosshairsBottom;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "UI | Crosshairs", DisplayName="准心右边")
	class UTexture2D* CrosshairsRight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Fire", DisplayName="开火过热曲线")
	UCurveFloat* FireOverHeatCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Fire", DisplayName="开火计数衰变值曲线")
	UCurveFloat* FireCountDecayCurve;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Fire", DisplayName="过热恢复曲线")
	UCurveFloat* OverHeatRecoverCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Fire", DisplayName="过热恢复速率")
	float RecoverInterval;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Fire", DisplayName="开火计数减少时间速率")
	float FireCountReduceInterval;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Fire", DisplayName="开火计数最大值")
	float FireCountMax;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Fire", DisplayName="开火计数最小值", meta=(UIMin=0))
	float FireCountMin;

	UPROPERTY(BlueprintReadOnly, Category="Fire", DisplayName="开火计数")
	float FireCount;

	UPROPERTY(BlueprintReadOnly, Category="Fire", DisplayName="开火计数减少定时器")
	FTimerHandle FireCountReduceTimerHandle;

	UPROPERTY(BlueprintReadOnly, Category="Fire", DisplayName="过热恢复定时器")
	FTimerHandle RecoverTimerHandle;

	UPROPERTY(BlueprintReadOnly, Category="Fire", DisplayName="允许过热恢复定时器")
	FTimerHandle AllowRecoverTimerHandle;

	UPROPERTY(BlueprintReadOnly, Category="Fire", DisplayName="是否允许过热恢复")
	bool bIsAllowRecover;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Fire", DisplayName="允许过热恢复的延迟时间")
	float DelayAllowRecoverTime;

	UPROPERTY(BlueprintReadOnly, Category="Fire", DisplayName="当前准星扩散值")
	float CurrentSpreadValue;
	
public:
	virtual void BeginPlay() override;
	
	virtual void Init(ABlasterCharacter* InPlayerCharacter) override;
	virtual void ShowCollimation() override;
	virtual void HideCollimation() override;
	
	UFUNCTION(Category="Crosshair", DisplayName="增加开火计数")
	void AddFireCount();
	
private:
	UFUNCTION(Category="Crosshair", DisplayName="开火计数减少处理")
	void FireCountReduceHandle();

	UFUNCTION(Category="Crosshair", DisplayName="过热恢复处理")
	void FireOverheatRecoverHandle();

	// 十字准星扩散
	UFUNCTION(DisplayName="更新准星扩散")
	virtual void UpdateSpread(float SpreadCurveValue);

	UFUNCTION(Category="Fire", DisplayName="允许过热恢复处理")
	void AllowRecoverHandle();
};
