// Fill out your copyright notice in the Description page of Project Settings.


#include "SteppedSlider.h"

#include "Components/Image.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/SizeBox.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "UMGVault/MyGameInstance.h"

// step 개수 계산
// step마다 TextBlock을 추가하기
// 룰? 균일(최소, 최대, 스탭수) 또는 커스텀(커스텀 값 배열), 일단 커스텀으로 구현
// value와 percent가 있어야 hook이 가능할 것 같음

void USteppedSlider::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (ThumbBorder)
	{
		ThumbBorder->OnMouseButtonUpEvent.BindDynamic(this, &USteppedSlider::OnThumbMouseUp);
		ThumbBorder->OnMouseButtonDownEvent.BindDynamic(this, &USteppedSlider::OnThumbMouseDown);
		ThumbBorder->OnMouseMoveEvent.BindDynamic(this, &USteppedSlider::OnThumbMouseMove);
	}

	// test용으로 StepValues를 초기화
	/*StepValues.Empty();

	StepValues.Reserve(5);

	StepValues.Add(FSliderStepData(100.0f));
	StepValues.Add(FSliderStepData(200.5f));
	StepValues.Add(FSliderStepData(444.5f));
	StepValues.Add(FSliderStepData(560.75f));
	StepValues.Add(FSliderStepData(900.0f));*/
	
	StepValues.IsValidIndex(CurrentIdx) ? CurrentPercent = StepValues[CurrentIdx].Percent : CurrentPercent = 0.f;
	//UE_LOG(LogTemp, Warning, TEXT("USteppedSlider::NativePreConstruct CurrentIdx: %d, CurrentPercent: %.2f"), CurrentIdx, CurrentPercent);

	UpdateTrackFillWidth(true);
	UpdateVisual(true);

	if (0 < StepsGroup->GetChildrenCount())
		StepsGroup->ClearChildren();

	GenerateSteps();
}

void USteppedSlider::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (bInitialize && UpdateTrackFillWidth(false) && UpdateVisual(false))
	{
		GenerateSteps();
		bInitialize = false;
		//UE_LOG(LogTemp, Warning, TEXT("USteppedSlider::NativeTick called and properties updated."));
	}
}

void USteppedSlider::GenerateSteps()
{
	// StepValues 배열을 기반으로 StepTextBlocks를 생성
	const int32 StepCount = StepValues.Num();

	if (StepCount > 1)
	{
		for (int32 ii = 0; ii < StepCount; ++ii)
		{
			FSliderStepData& StepValue = StepValues[ii];
			StepValue.Percent = float(ii) / (StepCount - 1);
		}
	}

	for (FSliderStepData& StepValue : StepValues)
	{
		// TextBlock이 nullptr이 아닌지 확인
		UTextBlock* NewStepTextBlock = nullptr;
		if (StepValue.TextBlock == nullptr)
		{
			NewStepTextBlock = NewObject<UTextBlock>(this);
			StepValue.TextBlock = NewStepTextBlock;
		}
		else
		{
			NewStepTextBlock = StepValue.TextBlock;
		}
		StepsGroup->AddChild(NewStepTextBlock);

		NewStepTextBlock->SetText(FText::FromString(FString::Printf(TEXT("%s"), *FloatToStringTrunc(StepValue.Value,2))));
		if (UOverlaySlot* OverlaySlot = Cast<UOverlaySlot>(NewStepTextBlock->Slot))
		{
			OverlaySlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
		}

		float PosX = (TrackFillWidth * StepValue.Percent) - (TrackFillWidth / 2);
		FWidgetTransform StepTextBlockWidgetTransform = NewStepTextBlock->GetRenderTransform();
		StepTextBlockWidgetTransform.Translation = FVector2D(PosX, 0.f);
		NewStepTextBlock->SetRenderTransform(StepTextBlockWidgetTransform);

		FSlateFontInfo NewFontInfo = NewStepTextBlock->Font;
		NewFontInfo.Size = 8;
		NewStepTextBlock->SetFont(NewFontInfo);

		NewStepTextBlock->SetVisibility(ESlateVisibility::Visible);

	}

	for (FSliderStepData& StepValue : StepValues)
	{
		if (StepValue.TextBlock == nullptr)
		{
			//UE_LOG(LogTemp, Warning, TEXT("StepValue.TextBlock is nullptr for value: %.2f"), StepValue.Value);
			continue; // TextBlock이 nullptr인 경우는 건너뜀
		}
		//UE_LOG(LogTemp, Warning, TEXT("%s"), *StepValue.TextBlock->GetName());
		//UE_LOG(LogTemp, Warning, TEXT("StepValue: Value=%.2f, Percent=%.2f"), StepValue.Value, StepValue.Percent);
		//UE_LOG(LogTemp, Warning, TEXT("StepTextBlock Render Transform: %s"), *StepValue.TextBlock->GetRenderTransform().Translation.ToString());
	}
	
	if (UMaterialInstanceDynamic* TrackFillMat = TrackFill->GetDynamicMaterial())
	{
		TrackFillMat->SetScalarParameterValue(FName("TickCount"), (StepCount - 1));
	}
}

bool USteppedSlider::UpdateTrackFillWidth(bool bPreview)
{
	// 레이아웃이 변경되면 UpdateTrackFillWidth()가 호출되어야 함
	// 패딩이나 마진이 있는 경우, 추가적인 계산이 필요할 수 있음
	if (bPreview)
	{
		TrackFillWidth = SizeBox->GetWidthOverride();
	}
	else
	{
		TrackFillWidth = SizeBox->GetCachedGeometry().GetLocalSize().X;
	}

	if (TrackFillWidth <= 0.f)
	{
		// UE_LOG(LogTemp, Warning, TEXT("USliderControl::UpdateVisual Width is zero!"));
		return false;
	}
	return true;
}

bool USteppedSlider::UpdateVisual(bool bPreview)
{
	if (TrackFillWidth <= 0.f)
	{
		// UE_LOG(LogTemp, Warning, TEXT("USliderControl::UpdateVisual Width is zero!"));
		return false;
	}

	const int32 StepCount = StepValues.Num();
	int32 FindIdx = 0;
	for(int32 ii = 0; ii < StepCount; ++ii)
	{
		FSliderStepData& StepValue = StepValues[ii];
		if (StepValue.Percent <= CurrentPercent)
		{
			FindIdx = ii;
		}
		else
		{
			break;
		}
	}

	if (FindIdx == StepCount - 1)
	{
		CurrentIdx = FindIdx;
	}
	else if (0 <= FindIdx)
	{
		const int32 PrevIdx = FindIdx;
		const int32 NextIdx = FindIdx + 1;
		const float CalcThresh = ((StepValues[NextIdx].Percent - StepValues[PrevIdx].Percent) / 2) * HookThresh;
		if (FMath::Abs(CurrentPercent - StepValues[PrevIdx].Percent) <= CalcThresh)
		{
			CurrentIdx = PrevIdx;
		}
		else if (FMath::Abs(CurrentPercent - StepValues[NextIdx].Percent) <= CalcThresh)
		{
			CurrentIdx = NextIdx;
		}
	}

	if (0 < StepCount)
	{
		CurrentPercent = StepValues[CurrentIdx].Percent;
	}

	//UE_LOG(LogTemp, Warning, TEXT("CurrentPercent: %f, FindIdx: %d"), CurrentPercent, FindIdx);

	float TumbPosX = (TrackFillWidth * CurrentPercent) - (TrackFillWidth / 2.f);
	FWidgetTransform ThumbBorderWidgetTransform = ThumbBorder->GetRenderTransform();
	ThumbBorderWidgetTransform.Translation = FVector2D(TumbPosX, 0.f);
	ThumbBorder->SetRenderTransform(ThumbBorderWidgetTransform);

	if (UMaterialInstanceDynamic* TrackFillMat = TrackFill->GetDynamicMaterial())
	{
		TrackFillMat->SetScalarParameterValue(FName("Percent"), (CurrentPercent));
	}

	LabelValue->SetText(FText::FromString(FString::Printf(TEXT("%s"), *FloatToStringTrunc(StepValues[CurrentIdx].Value, 2))));

	return true;
}

FEventReply USteppedSlider::OnThumbMouseDown(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
	bIsDragging = true;
	FEventReply Reply = UWidgetBlueprintLibrary::Handled();
	Reply = UWidgetBlueprintLibrary::CaptureMouse(Reply, ThumbBorder);
	return Reply;
}

FEventReply USteppedSlider::OnThumbMouseUp(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
	if (!bIsDragging)
		return UWidgetBlueprintLibrary::Unhandled();

	bIsDragging = false;

	FEventReply Reply = UWidgetBlueprintLibrary::Handled();
	return UWidgetBlueprintLibrary::ReleaseMouseCapture(Reply);
}

FEventReply USteppedSlider::OnThumbMouseMove(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
	if (bIsDragging)
	{
		FVector2D GlobalPosition = MouseEvent.GetScreenSpacePosition();
		FVector2D LocalPosition = TrackFill->GetCachedGeometry().AbsoluteToLocal(GlobalPosition);

		// 전체 길이에 대한 비율 계산
		// TODO: MyGeometry는 현재 위젯의 크기를 나타내므로, TrackFill의 크기를 사용해야 함
		CurrentPercent = FMath::Clamp((LocalPosition.X) / TrackFillWidth, 0.f, 1.f);

		//UE_LOG(LogTemp, Warning, TEXT("Mouse Position: %s, Local Position: %s"), *GlobalPosition.ToString(), *LocalPosition.ToString());
		//UE_LOG(LogTemp, Warning, TEXT("LocalPosition.X: %f, MyGeometry.GetLocalSize().X: %f"), LocalPosition.X, MyGeometry.GetLocalSize().X);
		//UE_LOG(LogTemp, Warning, TEXT("CurrentPercent: %f"), CurrentPercent);

		//UE_LOG(LogTemp, Warning, TEXT("MinValue: %f, MaxValue: %f"), MinValue, MaxValue);
		UpdateVisual(false);
	}
	return UWidgetBlueprintLibrary::Unhandled();
}