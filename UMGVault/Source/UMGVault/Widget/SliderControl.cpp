// Fill out your copyright notice in the Description page of Project Settings.


#include "SliderControl.h"

#include "Rendering/SlateRenderTransform.h"

#include "UMGVault/MyGameInstance.h"

#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Slider.h"
#include "Components/Border.h"

bool USliderControl::UpdateVisual(bool bPreview)
{
	float Width = 0.f;
	if (bPreview)
	{
		Width = SizeBox->GetWidthOverride();
	}
	else
	{
		Width = SizeBox->GetCachedGeometry().GetLocalSize().X;
	}

	if (Width <= 0.f)
	{
		//UE_LOG(LogTemp, Warning, TEXT("USliderControl::UpdateVisual Width is zero!"));
		return false;
	}

	const float Percent = Slider->GetValue() / Slider->GetMaxValue();
	TrackFill->GetDynamicMaterial()->SetScalarParameterValue(FName("Percent"), Percent);

	float TumbPosX = (Width * Percent) - (Width / 2.f);
    FSlateRenderTransform MyRenderTransform(FVector2D(TumbPosX, 0.f));
    FWidgetTransform MyWidgetTransform = ThumbBorder->GetRenderTransform();
	MyWidgetTransform.Translation = MyRenderTransform.GetTranslation();

    ThumbBorder->SetRenderTransform(MyWidgetTransform);

	TextBlock->SetText(FText::FromString(FString::Printf(TEXT("%s%%"), *FloatToStringTrunc(Percent * 100.f, 2))));

	return true;
}

void USliderControl::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bInitialize && UpdateVisual(false))
	{
		bInitialize = false;
		// UE_LOG(LogTemp, Warning, TEXT("USliderControl::NativeTick called and properties updated."));
	}
	// UE_LOG(LogTemp, Warning, TEXT("USliderControl::NativeTick called"));
}
