// Copyright Epic Games, Inc. All Rights Reserved.

#include "CrazyCartsGameMode.h"
#include "CrazyCartsPlayerController.h"

ACrazyCartsGameMode::ACrazyCartsGameMode()
{
	PlayerControllerClass = ACrazyCartsPlayerController::StaticClass();
}
