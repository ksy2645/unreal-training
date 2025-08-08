// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuWidget.h"


void UMenuWidget::SetMenuInterface(IMenuInterface* InputMenuInterface)
{
    this->MenuInterface = InputMenuInterface;
}

void UMenuWidget::Setup()
{
    UWorld* World = GetWorld();
    if (!ensure(nullptr != World)) return;

    this->AddToViewport();
    this->SetIsFocusable(true);
    if (APlayerController* PlayerController = World->GetFirstPlayerController())
    {
        FInputModeUIOnly InputModeData;
        InputModeData.SetWidgetToFocus(this->TakeWidget());
        InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

        PlayerController->SetInputMode(InputModeData);
        //PlayerController->SetMouseCursorWidget(EMouseCursor::Default, MainMenu);
        PlayerController->SetShowMouseCursor(true);
    }
}

void UMenuWidget::Teardown()
{
    this->RemoveFromParent();

    UWorld* World = GetWorld();
    if (!ensure(nullptr != World)) return;

    if (APlayerController* PlayerController = World->GetFirstPlayerController())
    {
        FInputModeGameOnly InputModeData;
        PlayerController->SetInputMode(InputModeData);
        PlayerController->SetShowMouseCursor(false);
    }
}
