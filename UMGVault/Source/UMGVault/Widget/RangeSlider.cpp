// Fill out your copyright notice in the Description page of Project Settings.


#include "RangeSlider.h"

#include "Components/Image.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/SizeBox.h"
#include "Input/Reply.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

#include "UMGVault/MyGameInstance.h"



bool URangeSlider::Initialize()
{
	bool bSuccess = Super::Initialize();

	if (ThumbMinBorder)
	{
		ThumbMinBorder->OnMouseButtonDownEvent.BindDynamic(this, &URangeSlider::OnThumbMinMouseDown);
		ThumbMinBorder->OnMouseButtonUpEvent.BindDynamic(this, &URangeSlider::OnThumbMinMouseUp);
		ThumbMinBorder->OnMouseMoveEvent.BindDynamic(this, &URangeSlider::OnThumbMinMouseMove);
	}

	if (ThumbMaxBorder)
	{
		ThumbMaxBorder->OnMouseButtonDownEvent.BindDynamic(this, &URangeSlider::OnThumbMaxMouseDown);
		ThumbMaxBorder->OnMouseButtonUpEvent.BindDynamic(this, &URangeSlider::OnThumbMaxMouseUp);
		ThumbMaxBorder->OnMouseMoveEvent.BindDynamic(this, &URangeSlider::OnThumbMinMouseMove);
	}

	//UE_LOG(LogTemp, Warning, TEXT("URangeSlider::Initialize called!"));
	return bSuccess;
}

void URangeSlider::NativePreConstruct()
{
	Super::NativePreConstruct();

	//UE_LOG(LogTemp, Warning, TEXT("URangeSlider::PreConstruct called!"));
	// UKismetSystemLibrary::PrintString(this, FString(L"URangeSlider::PreConstruct called!"));

	ForceLayoutPrepass();
	UpdateVisual(true);

	AbsMin->SetText(FText::FromString(FString::Printf(TEXT("%s"), *FloatToStringTrunc(AbsoluteMinValue, 2))));
	AbsMax->SetText(FText::FromString(FString::Printf(TEXT("%s"), *FloatToStringTrunc(AbsoluteMaxValue, 2))));
}

FEventReply URangeSlider::OnThumbMinMouseDown(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
	//UE_LOG(LogTemp, Warning, TEXT("ThumbMinMouseDown event triggered!"));
	bIsDraggingMin = true;
	FEventReply Reply = UWidgetBlueprintLibrary::Handled();
	Reply = UWidgetBlueprintLibrary::CaptureMouse(Reply, ThumbMinBorder);

	return Reply;
}

FEventReply URangeSlider::OnThumbMinMouseUp(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
	//UE_LOG(LogTemp, Warning, TEXT("OnThumbMinMouseUp event triggered!"));
	
	if (!bIsDraggingMin)
		return UWidgetBlueprintLibrary::Unhandled();

	bIsDraggingMin = false;

	// 마우스 캡처 해제
	FEventReply Reply = UWidgetBlueprintLibrary::Handled();
	return UWidgetBlueprintLibrary::ReleaseMouseCapture(Reply);
}

FEventReply URangeSlider::OnThumbMaxMouseDown(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
	//UE_LOG(LogTemp, Warning, TEXT("OnThumbMaxMouseDown event triggered!"));
	bIsDraggingMax = true;
	FEventReply Reply = UWidgetBlueprintLibrary::Handled();
	Reply = UWidgetBlueprintLibrary::CaptureMouse(Reply, ThumbMaxBorder);

	return Reply;
}

FEventReply URangeSlider::OnThumbMaxMouseUp(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
	//UE_LOG(LogTemp, Warning, TEXT("OnThumbMaxMouseUp event triggered!"));
	
	if (!bIsDraggingMax)
		return UWidgetBlueprintLibrary::Unhandled();

	bIsDraggingMax = false;

	// 마우스 캡처 해제
	FEventReply Reply = UWidgetBlueprintLibrary::Handled();
	return UWidgetBlueprintLibrary::ReleaseMouseCapture(Reply);
}

FEventReply URangeSlider::OnThumbMinMouseMove(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
	if (bIsDraggingMin || bIsDraggingMax)
	{
		// UE_LOG(LogTemp, Warning, TEXT("OnThumbMinMouseMove event triggered!"));
		FVector2D GlobalPosition = MouseEvent.GetScreenSpacePosition();
		FVector2D LocalPosition = TrackFill->GetCachedGeometry().AbsoluteToLocal(GlobalPosition);
		
		// 전체 길이에 대한 비율 계산
		// TODO: MyGeometry는 현재 위젯의 크기를 나타내므로, TrackFill의 크기를 사용해야 함
		float Percent = FMath::Clamp((LocalPosition.X) / TrackFillWidth, 0.f, 1.f);

		//UE_LOG(LogTemp, Warning, TEXT("Mouse Position: %s, Local Position: %s"), *GlobalPosition.ToString(), *LocalPosition.ToString());
		//UE_LOG(LogTemp, Warning, TEXT("LocalPosition.X: %f, MyGeometry.GetLocalSize().X: %f"), LocalPosition.X, MyGeometry.GetLocalSize().X);
		//UE_LOG(LogTemp, Warning, TEXT("Percent: %f"), Percent);

		if (bIsDraggingMin)
		{
			// value 범위 제한 값
			// TODO: 절대 최소값과 최대값을 변수로 설정하여 사용, 0.0f, 100.0f를 상수로 사용하지 않도록 변경
			MinValue = FMath::Lerp(AbsoluteMinValue, AbsoluteMaxValue, Percent);
		}
		else
		{
			MaxValue = FMath::Lerp(AbsoluteMinValue, AbsoluteMaxValue, Percent);
		}

		//UE_LOG(LogTemp, Warning, TEXT("MinValue: %f, MaxValue: %f"), MinValue, MaxValue);
		UpdateVisual(false);
	}

	return UWidgetBlueprintLibrary::Unhandled();
}


void URangeSlider::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bInitialize && UpdateVisual(false))
	{
		bInitialize = false;
		// UE_LOG(LogTemp, Warning, TEXT("USliderControl::NativeTick called and properties updated."));


	}
	// UE_LOG(LogTemp, Warning, TEXT("USliderControl::NativeTick called"));
}

bool URangeSlider::UpdateVisual(bool bPreview)
{
	if (bPreview)
	{
		TrackFillWidth = SizeBox->GetWidthOverride();
	}
	else
	{
		TrackFillWidth = SizeBox->GetCachedGeometry().GetLocalSize().X;
	}

	//bool bt = true;
	//if (bt)
	//	return true;
	if (TrackFillWidth <= 0.f)
	{
		//UE_LOG(LogTemp, Warning, TEXT("USliderControl::UpdateVisual Width is zero!"));
		return false;
	}

	const float MinPercent = (MinValue - AbsoluteMinValue) / (AbsoluteMaxValue - AbsoluteMinValue);
	const float MaxPercent = (MaxValue - AbsoluteMinValue) / (AbsoluteMaxValue - AbsoluteMinValue);

	//UE_LOG(LogTemp, Warning, TEXT("USliderControl::UpdateVisual MinPercent: %f, MaxPercent: %f"), MinPercent, MaxPercent);

	if (MinPercent <= MaxPercent)
	{
		TrackFill->GetDynamicMaterial()->SetScalarParameterValue(FName("MinPercent"), MinPercent);
		TrackFill->GetDynamicMaterial()->SetScalarParameterValue(FName("MaxPercent"), MaxPercent);
	}
	else
	{
		TrackFill->GetDynamicMaterial()->SetScalarParameterValue(FName("MinPercent"), MaxPercent);
		TrackFill->GetDynamicMaterial()->SetScalarParameterValue(FName("MaxPercent"), MinPercent);
	}

	LabelMin->SetText(FText::FromString(FString::Printf(TEXT("Percent: %s%%\nValue: %s"), *FloatToStringTrunc(MinPercent * 100.f, 2), *FloatToStringTrunc(MinValue, 2))));
	LabelMax->SetText(FText::FromString(FString::Printf(TEXT("Percent: %s%%\nValue: %s"), *FloatToStringTrunc(MaxPercent * 100.f, 2), *FloatToStringTrunc(MaxValue, 2))));

	float TumbPosX = (TrackFillWidth * MinPercent) - (TrackFillWidth / 2.f);
	FSlateRenderTransform MinBorderRenderTransform(FVector2D(TumbPosX, 0.f));
	FWidgetTransform MinBorderWidgetTransform = ThumbMinBorder->GetRenderTransform();
	MinBorderWidgetTransform.Translation = MinBorderRenderTransform.GetTranslation();

	ThumbMinBorder->SetRenderTransform(MinBorderWidgetTransform);

	float MaxTumbPosX = (TrackFillWidth * MaxPercent) - (TrackFillWidth / 2.f);
	FSlateRenderTransform MaxBorderRenderTransform(FVector2D(MaxTumbPosX, 0.f));
	FWidgetTransform MaxBorderWidgetTransform = ThumbMaxBorder->GetRenderTransform();
	MaxBorderWidgetTransform.Translation = MaxBorderRenderTransform.GetTranslation();

	ThumbMaxBorder->SetRenderTransform(MaxBorderWidgetTransform);

	return true;
}

//
//FReply URangeSlider::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
//{
//	UE_LOG(LogTemp, Warning, TEXT("NativeOnMouseButtonUp!"));
//	if (bIsDraggingMin)
//	{

//		bIsDraggingMin = false;
//		UE_LOG(LogTemp, Warning, TEXT("Mouse button released on ThumbMinBorder!"));
//		
//		// 마우스 해제
//		return FReply::Handled().ReleaseMouseCapture();
//	}
//	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
//}

float URangeSlider::GetMinValue() const
{
	if (MinValue <= MaxValue)
		return MinValue;
			
	return MaxValue;
}

float URangeSlider::GetMaxValue() const
{
	if (MinValue <= MaxValue)
		return MaxValue;

	return MinValue;
}
