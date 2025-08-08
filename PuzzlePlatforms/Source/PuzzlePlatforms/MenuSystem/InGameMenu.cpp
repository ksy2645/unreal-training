// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameMenu.h"

#include "Components/Button.h"

#include "MenuInterface.h"

bool UInGameMenu::Initialize()
{
    bool Success = Super::Initialize();
    if (false == Success) return false;

    if (!ensure(nullptr != CancelButton)) return false;
    CancelButton->OnClicked.AddUniqueDynamic(this, &UInGameMenu::CancelButtonOnClick);

    if (!ensure(nullptr != QuitButton)) return false;
    QuitButton->OnClicked.AddUniqueDynamic(this, &UInGameMenu::QuitButtonOnClick);

    return true;
}

void UInGameMenu::CancelButtonOnClick()
{
    Teardown();
}

void UInGameMenu::QuitButtonOnClick()
{
    if (nullptr == MenuInterface) return;
    Teardown();
    MenuInterface->LoadMainMenu();
}
