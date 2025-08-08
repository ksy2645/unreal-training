// Fill out your copyright notice in the Description page of Project Settings.


#include "RotaryKnob.h"

#include "Components/Overlay.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "Blueprint/WidgetBlueprintLibrary.h"

#include "UMGVault/MyGameInstance.h"

void URotaryKnob::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (RotaryGroup)
	{
		RotaryGroup->OnMouseButtonDownEvent.BindDynamic(this, &URotaryKnob::OnRotaryMouseDown);
		RotaryGroup->OnMouseButtonUpEvent.BindDynamic(this, &URotaryKnob::OnRotaryMouseUp);
		RotaryGroup->OnMouseMoveEvent.BindDynamic(this, &URotaryKnob::OnRotaryMouseMove);
	}

	InitKnob();
	UpdateVisual();
}

FEventReply URotaryKnob::OnRotaryMouseDown(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
	bIsRotating = true;
	FEventReply Reply = UWidgetBlueprintLibrary::Handled();
	UWidgetBlueprintLibrary::CaptureMouse(Reply, RotaryGroup);
	return Reply;
}

FEventReply URotaryKnob::OnRotaryMouseUp(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
	if (!bIsRotating)
		return UWidgetBlueprintLibrary::Unhandled();

	bIsRotating = false;
	FEventReply Reply = UWidgetBlueprintLibrary::Handled();
	return UWidgetBlueprintLibrary::ReleaseMouseCapture(Reply);
}

FEventReply URotaryKnob::OnRotaryMouseMove(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
	if (bIsRotating)
	{
		FVector2D GlobalMousePostion = MouseEvent.GetScreenSpacePosition();
		FVector2D GlobalBorderPostion = MyGeometry.GetAbsolutePosition() + (MyGeometry.GetAbsoluteSize() / 2); // border의 중앙 좌표 기준
		
		//FVector2D LocalMousePosition = MyGeometry.AbsoluteToLocal(GlobalMousePostion); // border의 왼쪽 위 좌표기준
		//UE_LOG(LogTemp, Warning, TEXT("OnRotaryMouseMove::BorderPostion: %s"), *GlobalBorderPostion.ToString());
		//UE_LOG(LogTemp, Warning, TEXT("OnRotaryMouseMove::MousePostion: %s"), *GlobalMousePostion.ToString());
		//UE_LOG(LogTemp, Warning, TEXT("OnRotaryMouseMove::LocalMousePostion: %s"), *LocalMousePosition.ToString());
		FVector2D CustomPosition = GlobalMousePostion - GlobalBorderPostion;
		//UE_LOG(LogTemp, Warning, TEXT("OnRotaryMouseMove::CustomPosition: %s"), *CustomPosition.ToString());
		//float Angle = ((FMath::Atan2(CustomPosition.Y, CustomPosition.X) + PI) / (2 * PI)) * 360;
		//UE_LOG(LogTemp, Warning, TEXT("OnRotaryMouseMove::Angle: %f"), Angle);
		FVector2D CustomPosition2 = RotatedPosition2D(360.f - StartAngle, CustomPosition); // 시작 각도 기준으로 회전
		float OrgAngle = FMath::RadiansToDegrees(FMath::Atan2(CustomPosition2.Y, CustomPosition2.X)) + 180;
		//UE_LOG(LogTemp, Warning, TEXT("OnRotaryMouseMove::TAngle: %f"), TAngle);
		//FMath::Lerp(StartAngle, EndAngle, TAngle / 360.f);
		//315 -> 225 => 270
		//360 -> 225 => 225
		//0 -> 225 => 225
		float RangeValue = FMath::Fmod((EndAngle - StartAngle + 360.f), 360.f);
		//float Percent = FMath::Clamp(TAngle / RangeValue, 0.f, 1.f);
		float Percent = OrgAngle / RangeValue;
		const float ThresholdValue = (((360.f - RangeValue) / RangeValue) * 0.5f) * ThresholdRate;
		const float MaxPercent = 360.f / RangeValue;
		//UE_LOG(LogTemp, Warning, TEXT("OnRotaryMouseMove::RangeValue: %f"), RangeValue);
		//UE_LOG(LogTemp, Warning, TEXT("OnRotaryMouseMove::Percent: %f"), Percent);

		if (1.f <= Percent)
		{
			if (MaxPercent - ThresholdValue <= Percent)
			{
				CurrentPercent = 0.f;
			}
			else if (Percent <= 1.f + ThresholdValue)
			{
				CurrentPercent = 1.f;
			}
		}
		else
		{
			CurrentPercent = Percent;
		}

		//UE_LOG(LogTemp, Warning, TEXT("OnRotaryMouseMove::CurrentPercent: %f"), CurrentPercent);

		UpdateVisual();
	}
		
	return UWidgetBlueprintLibrary::Unhandled();
}

void URotaryKnob::UpdateVisual()
{
	if (UMaterialInstanceDynamic* DyMat = TrackFill->GetDynamicMaterial())
	{
		DyMat->SetScalarParameterValue(FName("Percent"), CurrentPercent);
		//UE_LOG(LogTemp, Warning, TEXT("UpdateVisual::CurrentPercent: %f"), CurrentPercent);
	}
	float RangeValue = FMath::Fmod((EndAngle - StartAngle + 360.f), 360.f);
	//// CurrentPercent = OrgAngle / RangeValue; => OrgValue = CurrentPercent * RangeValue;
	//float AngleValue = CurrentPercent * RangeValue;
	//// -180 -> 180
	//// 315.f -> -45.f, 45.f -> 45.f
	//// fmod(StartAngle + 180.f, 360.f) - 180.f
	//// 180도 보정 -> -180.f
	//float NormStartDeg = FMath::Fmod(StartAngle + 180.f, 360.f) - 360.f;
	//UE_LOG(LogTemp, Warning, TEXT("UpdateVisual::AbsStartDeg: %f"), NormStartDeg);
	//RotaryGroup->SetRenderTransformAngle(AngleValue + NormStartDeg);

	float DeltaAngle = FMath::UnwindDegrees(StartAngle);
	float Angle = DeltaAngle + (RangeValue * CurrentPercent) - 180.f;
	RotaryGroup->SetRenderTransformAngle(Angle);

	ValueText->SetText(FText::FromString(FloatToStringTrunc(CurrentPercent * 100.f, 2) + TEXT("%")));
}

void URotaryKnob::InitKnob()
{
	if (UMaterialInstanceDynamic* DyMat = TrackFill->GetDynamicMaterial())
	{
		DyMat->SetScalarParameterValue(FName("StartDeg"), StartAngle);
		DyMat->SetScalarParameterValue(FName("EndDeg"), EndAngle);
		DyMat->SetScalarParameterValue(FName("TickCount"), VisualTickCount);
		//DyMat->SetScalarParameterValue(FName("Percent"), CurrentPercent);
		//UE_LOG(LogTemp, Warning, TEXT("InitKnob::CurrentPercent: %f"), CurrentPercent);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("RotaryKnob::InitKnob::DyMat is nullptr"));
	}
}
