#pragma once

#include "CoreMinimal.h"
#include "Components/TextBlock.h"
#include "SliderStepData.generated.h"

USTRUCT(BlueprintType)
struct FSliderStepData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Value = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Percent = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTextBlock> TextBlock = nullptr;
	FSliderStepData() = default;

	FSliderStepData(float InValue)
		: Value(InValue)
	{
	}
	FSliderStepData(float InValue, float InPercent, TObjectPtr<UTextBlock> InTextBlock)
		: Value(InValue)
		, Percent(InPercent)
		, TextBlock(InTextBlock)
	{
	}
};