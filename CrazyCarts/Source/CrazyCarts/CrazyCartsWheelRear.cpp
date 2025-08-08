// Copyright Epic Games, Inc. All Rights Reserved.

#include "CrazyCartsWheelRear.h"
#include "UObject/ConstructorHelpers.h"

UCrazyCartsWheelRear::UCrazyCartsWheelRear()
{
	AxleType = EAxleType::Rear;
	bAffectedByHandbrake = true;
	bAffectedByEngine = true;
}