// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RotaryKnob.generated.h"

/**
 * 
 */
UCLASS()
class UMGVAULT_API URotaryKnob : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativePreConstruct() override;

	UFUNCTION() FEventReply OnRotaryMouseDown(FGeometry MyGeometry, const FPointerEvent& MouseEvent);
	UFUNCTION() FEventReply OnRotaryMouseUp(FGeometry MyGeometry, const FPointerEvent& MouseEvent);
	UFUNCTION() FEventReply OnRotaryMouseMove(FGeometry MyGeometry, const FPointerEvent& MouseEvent);
	void UpdateVisual();
	void InitKnob();
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	class UBorder* RotaryGroup;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* TrackFill;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* ValueText;

	UPROPERTY(EditAnywhere, Category = "RotaryKnob")
	float CurrentPercent = 0.f;
	UPROPERTY(EditAnywhere, Category = "RotaryKnob")
	float StartAngle = 315.f; // 시작 각도
	UPROPERTY(EditAnywhere, Category = "RotaryKnob")
	float EndAngle = 225.f; // 끝 각도
	UPROPERTY(EditAnywhere, Category = "RotaryKnob")
	float ThresholdRate = 0.5f; // Percent = 0 또는 1에 달라붙는 역치비율
	UPROPERTY(EditAnywhere, Category = "RotaryKnob")
	int32 VisualTickCount = 10; // 시각적 업데이트 카운트

private:
	bool bIsRotating = false;
};


/*
* StartAngle, AngleRange 두 값으로 구현하는게 사용할 때 더 편할 것 같음.
*/
