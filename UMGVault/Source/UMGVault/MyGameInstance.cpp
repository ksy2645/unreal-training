// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"
#include "Widget/SliderControl.h"

void UMyGameInstance::TestFunction()
{
}

FString FloatToStringTrunc(float V, int32 Precisiion)
{
	FString str = FString::Printf(TEXT("%.*f"), Precisiion, V);
	while ((str.Len() > 0) && (str[str.Len() - 1] == '0'))
	{
		// UE_LOG(LogTemp, Warning, TEXT("%d, %c"), str.Len() - 1, str[str.Len() - 1]);
		str.RemoveAt(str.Len() - 1);
	}

	if (str.Len() > 0 && str[str.Len() - 1] == '.')
	{
		str.RemoveAt(str.Len() - 1);
	}

	// UE_LOG(LogTemp, Warning, TEXT("truncate2: %s"), *str);
	return str;
}

FVector2D RotatedPosition2D(const float AngleDeg, const FVector2D& InPosition)
{
	float AngleRad = FMath::DegreesToRadians(AngleDeg);

	float s = FMath::Sin(AngleRad);
	float c = FMath::Cos(AngleRad);

	FVector2D OutPosition;
	OutPosition.X = c * InPosition.X - s * InPosition.Y;
	OutPosition.Y = s * InPosition.X + c * InPosition.Y;

	return OutPosition;
}

