// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "SliderStepData.h"

#include "SteppedSlider.generated.h"


class UTextBlock;
/**
 * 
 */
UCLASS()
class UMGVAULT_API USteppedSlider : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION() FEventReply OnThumbMouseDown(FGeometry MyGeometry, const FPointerEvent& MouseEvent);
	UFUNCTION() FEventReply OnThumbMouseUp(FGeometry MyGeometry, const FPointerEvent& MouseEvent);
	UFUNCTION() FEventReply OnThumbMouseMove(FGeometry MyGeometry, const FPointerEvent& MouseEvent);

	UFUNCTION(BluePrintCallable)
	bool UpdateVisual(bool bPreview);
	float GetValue() const { return StepValues.IsValidIndex(CurrentIdx) ? StepValues[CurrentIdx].Value : 0.0f; }

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	void GenerateSteps();
	bool UpdateTrackFillWidth(bool bPreview);

	//UPROPERTY(BlueprintReadWrite)
	//int32 StepCount = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSliderStepData> StepValues;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* TrackFill;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* LabelValue;

	UPROPERTY(meta = (BindWidget))
	class UBorder* ThumbBorder;

	UPROPERTY(meta = (BindWidget))
	class USizeBox* SizeBox;

	UPROPERTY(meta = (BindWidget))
	class UOverlay* StepsGroup;

private:
	UPROPERTY(EditAnywhere)
	int32 CurrentIdx = 0; // 현재 선택된 스텝 인덱스
	UPROPERTY(EditAnywhere)
	float HookThresh = 0.5f; // 끈적임을 위한 임계값

	float CurrentPercent = 0.0f; // 계산 
	float TrackFillWidth = 0.0f;
	bool bInitialize = true;
	bool bIsDragging = false;
};
