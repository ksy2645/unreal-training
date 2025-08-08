// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzlePlayerController.h"


void APuzzlePlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (IsLocalController())
    {
        //UE_LOG(LogTemp, Warning, TEXT("Local Player BeginPlay"));

        UE_LOG(LogTemp, Warning, TEXT("FInputModeGameOnly"));
        FInputModeGameOnly InputModeData;
        SetInputMode(InputModeData);
        SetShowMouseCursor(false);
    }
}
