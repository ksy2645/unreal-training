// Copyright Epic Games, Inc. All Rights Reserved.

#include "CrazyCartsWheelFront.h"
#include "UObject/ConstructorHelpers.h"

UCrazyCartsWheelFront::UCrazyCartsWheelFront()
{
	AxleType = EAxleType::Front;
	bAffectedBySteering = true;
	MaxSteerAngle = 40.f;
}