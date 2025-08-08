// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SliderControl.generated.h"

/**
 * 
 */
UCLASS()
class UMGVAULT_API USliderControl : public UUserWidget
{
	GENERATED_BODY()
public:
	void CustomTest()
	{
		UE_LOG(LogTemp, Warning, TEXT("USliderControl::test Called"));
	}

	UFUNCTION(BlueprintCallable, Category = "SliderControl")
	bool UpdateVisual(bool bPreview);

	float GetValue() const { return Value; }

protected:

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Value")
	float Value = 0.7f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Value")
	float MaxValue = 1.f;

	bool bInitialize = true;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class USizeBox* SizeBox;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UImage* TrackFill;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UBorder* ThumbBorder;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UTextBlock* TextBlock;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class USlider* Slider;
};
