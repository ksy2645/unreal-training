// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PuzzlePlatformsGameMode.generated.h"

UCLASS(minimalapi)
class APuzzlePlatformsGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APuzzlePlatformsGameMode();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	virtual void BeginPlay() override;
};



