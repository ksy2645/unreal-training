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

FString FloatToStringTrunc(float V, int32 Precisiion);
FVector2D RotatedPosition2D(float Angle, const FVector2D& InPosition);