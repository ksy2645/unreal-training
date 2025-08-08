// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Input/Reply.h"

#include "RangeSlider.generated.h"
/**
 * 
 */
UCLASS()
class UMGVAULT_API URangeSlider : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual bool Initialize() override;

	UFUNCTION() FEventReply OnThumbMinMouseDown(FGeometry MyGeometry, const FPointerEvent& MouseEvent);
	UFUNCTION() FEventReply OnThumbMinMouseUp(FGeometry MyGeometry, const FPointerEvent& MouseEvent);
	UFUNCTION() FEventReply OnThumbMinMouseMove(FGeometry MyGeometry, const FPointerEvent& MouseEvent);

	UFUNCTION() FEventReply OnThumbMaxMouseDown(FGeometry MyGeometry, const FPointerEvent& MouseEvent);
	UFUNCTION() FEventReply OnThumbMaxMouseUp(FGeometry MyGeometry, const FPointerEvent& MouseEvent);

	UFUNCTION(BluePrintCallable)
	bool UpdateVisual(bool bPreview);

	float GetMinValue() const;
	float GetMaxValue() const;

protected:
	//virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativePreConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* TrackFill;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* LabelMin;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* LabelMax;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* AbsMin;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* AbsMax;

private:

	UPROPERTY(meta = (BindWidget))
	class UBorder* ThumbMinBorder;

	UPROPERTY(meta = (BindWidget))
	class UBorder* ThumbMaxBorder;

	UPROPERTY(meta = (BindWidget))
	class USizeBox* SizeBox;

	UPROPERTY(EditAnywhere) float AbsoluteMinValue = -200.0f;
	UPROPERTY(EditAnywhere) float AbsoluteMaxValue = 700.0f;
	UPROPERTY(EditAnywhere) float MinValue = 20.0f;
	UPROPERTY(EditAnywhere) float MaxValue = 500.0f;
	UPROPERTY(EditAnywhere)
	bool bInitialize = true;
	float TrackFillWidth = 0.0f;

	bool bIsDraggingMin = false;
	bool bIsDraggingMax = false;

};