// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"


/**
*
 */
UCLASS()
class UMGVAULT_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UFUNCTION(Exec)
	void TestFunction();
};

inline FString FloatToStringTrunc(float V, int32 Precisiion)
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
inline FVector2D RotatedPosition2D(float AngleDeg, const FVector2D& InPosition)
{
	float AngleRad = FMath::DegreesToRadians(AngleDeg);

	float s = FMath::Sin(AngleRad);
	float c = FMath::Cos(AngleRad);

	FVector2D OutPosition;
	OutPosition.X = c * InPosition.X - s * InPosition.Y;
	OutPosition.Y = s * InPosition.X + c * InPosition.Y;

	return OutPosition;
}