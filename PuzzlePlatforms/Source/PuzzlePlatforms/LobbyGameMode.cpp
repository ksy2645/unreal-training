// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"

#include "TimerManager.h"

#include "PuzzlePlayerController.h"
#include "PuzzlePlatformsGameInstance.h"

ALobbyGameMode::ALobbyGameMode()
{
	PlayerControllerClass = APuzzlePlayerController::StaticClass();
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    ++NumberOfPlayers;

    if (2 <= NumberOfPlayers)
    {
        GetWorldTimerManager().SetTimer(GameStartTimer, this,  &ALobbyGameMode::StartGame, 5);
        
        // UE_LOG(LogTemp, Warning, TEXT("Reached 3 players!"));
    }
}

void ALobbyGameMode::Logout(AController* Exiting)
{
    Super::Logout(Exiting);

    --NumberOfPlayers;

    if (NumberOfPlayers < 2)
    {
        GetWorldTimerManager().ClearTimer(GameStartTimer);
    }
}

void ALobbyGameMode::StartGame()
{
    UPuzzlePlatformsGameInstance* GameInstance = Cast<UPuzzlePlatformsGameInstance>(GetGameInstance());
    if (nullptr == GameInstance) return;

    GameInstance->StartSession();

    UWorld* World = GetWorld();
    if (!ensure(nullptr != World)) return;

    bUseSeamlessTravel = true;
    World->ServerTravel("/Game/Maps/Game?listen");
}
