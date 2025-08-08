// Fill out your copyright notice in the Description page of Project Settings.


#include "FirstLevelGameMode.h"

#include "PuzzlePlayerController.h"

AFirstLevelGameMode::AFirstLevelGameMode()
{
    PlayerControllerClass = APuzzlePlayerController::StaticClass();
}
