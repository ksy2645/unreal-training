// Fill out your copyright notice in the Description page of Project Settings.
#include "MainMenu.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"

#include "MenuInterface.h"
#include "ServerRow.h"

// #include "PuzzlePlatforms/PuzzlePlatformsGameInstance.h"

UMainMenu::UMainMenu(const FObjectInitializer& ObjectInitializer)
{
    ConstructorHelpers::FClassFinder<UUserWidget> ServerRowBPClass(TEXT("/Game/MenuSystem/WBP_ServerRow"));
    if (!ensure(nullptr != ServerRowBPClass.Class)) return;

    ServerRowClass = ServerRowBPClass.Class;
}

bool UMainMenu::Initialize()
{
    bool Success = Super::Initialize();

    if (!Success) return false;

    // TODO: setup
    if (!ensure(nullptr != HostButton)) return false;
    HostButton->OnClicked.AddUniqueDynamic(this, &UMainMenu::HostServer);
    
    if (!ensure(nullptr != JoinButton)) return false;
    JoinButton->OnClicked.AddUniqueDynamic(this, &UMainMenu::OpenJoinMenu);

    if (!ensure(nullptr != CancelJoinMenuButton)) return false;
    CancelJoinMenuButton->OnClicked.AddUniqueDynamic(this, &UMainMenu::OpenMainMenu);

    if (!ensure(nullptr != ConfirmJoinMenuButton)) return false;
    ConfirmJoinMenuButton->OnClicked.AddUniqueDynamic(this, &UMainMenu::ConfirmJoinMenuButtonClick);

    if (!ensure(nullptr != ExitButton)) return false;
    ExitButton->OnClicked.AddUniqueDynamic(this, &UMainMenu::ExitButtonOnClick);

    return Success;
}

void UMainMenu::SetServerList(TArray<FServerData> ServerDatas)
{
    if (!ensure(nullptr != ServerList)) return;

    UWorld* World = this->GetWorld();
    if (!ensure(nullptr != World)) return;

    ServerList->ClearChildren();

    uint32 i = 0;
    for (const FServerData& ServerData : ServerDatas)
    {
        UServerRow* Row = CreateWidget<UServerRow>(World, ServerRowClass);
        if (!ensure(nullptr != Row)) return;
    
        Row->ServerNameText->SetText(FText::FromString(ServerData.Name));
        Row->HostUser->SetText(FText::FromString(ServerData.HostUsername));
        FString FractionText = FString::Printf(TEXT("%d/%d"), ServerData.CurrentPlayers, ServerData.MaxPlayers);
        Row->ConnectionFraction->SetText(FText::FromString(FractionText));
        Row->Setup(this, i);
        ++i;
    
        ServerList->AddChild(Row);
    }
}

void UMainMenu::SelectIndex(uint32 Index)
{
    SelectedIndex = Index;
    // UE_LOG(LogTemp, Warning, TEXT("Selected index %d"), SelectedIndex.GetValue());
    UpdateChildren();
}

void UMainMenu::UpdateChildren()
{
    for (int32 ii = 0; ii < ServerList->GetChildrenCount(); ++ii)
    {
        UServerRow* Row = Cast<UServerRow>(ServerList->GetChildAt(ii));
        Row->Selected = (SelectedIndex.IsSet() && SelectedIndex.GetValue() == ii);
        if (Row->Selected)
        {
            Row->SetColorAndOpacity(FLinearColor::Yellow);
        }
        else
        {
            Row->SetColorAndOpacity(FLinearColor::White);
        }
    }
}

void UMainMenu::HostServer()
{
    UE_LOG(LogTemp, Warning, TEXT("i'm gonna host a server!"));

    FText HostName = HostNameText->GetText();
    if (nullptr != MenuInterface)
    {
        MenuInterface->Host(HostName);
    }
    // UPuzzlePlatformsGameInstance* GameInstance = Cast<UPuzzlePlatformsGameInstance>(GetGameInstance());
    // if (!ensure(nullptr != GameInstance)) return;

    // GameInstance->Host();
}

void UMainMenu::OpenJoinMenu()
{
    if (!ensure(nullptr != MenuSwitcher)) return;
    if (!ensure(nullptr != JoinMenu)) return;
    MenuSwitcher->SetActiveWidget(JoinMenu);

    if (nullptr != MenuInterface)
    {
        MenuInterface->FindSessions();
    }
}

void UMainMenu::OpenMainMenu()
{
    if (!ensure(nullptr != MenuSwitcher)) return;
    if (!ensure(nullptr != MainMenu)) return;
    MenuSwitcher->SetActiveWidget(MainMenu);
}

void UMainMenu::ConfirmJoinMenuButtonClick()
{
    UE_LOG(LogTemp, Warning, TEXT("i'm gonna join a server!"));

    if (true == SelectedIndex.IsSet() && nullptr != MenuInterface)
    {
        UE_LOG(LogTemp, Warning, TEXT("Try join Selected index %d Session"), SelectedIndex.GetValue());
        MenuInterface->Join(SelectedIndex.GetValue());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Selected index not set"));
    }
}

void UMainMenu::ExitButtonOnClick()
{
    UWorld* World = GetWorld();
    if (APlayerController* PlayerController = World->GetFirstPlayerController())
    {
        PlayerController->ConsoleCommand("quit");
    }
}

