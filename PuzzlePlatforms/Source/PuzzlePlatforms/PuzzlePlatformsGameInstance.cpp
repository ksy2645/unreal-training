// Fill out your copyright notice in the Description page of Project Settings.
#include "PuzzlePlatformsGameInstance.h"

#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"

#include "MenuSystem/MainMenu.h"
#include "MenuSystem/InGameMenu.h"
#include "PlatformTrigger.h"
#include "Online/OnlineSessionNames.h"

const static FName SESSION_NAME = TEXT("GameSession");
const static FName HOSTNAME_SETTING_KEY = TEXT("HostName");

UPuzzlePlatformsGameInstance::UPuzzlePlatformsGameInstance(const FObjectInitializer& ObjectInitializer)
{
    ConstructorHelpers::FClassFinder<UUserWidget> MainMenuWidgetBPClass(TEXT("/Game/MenuSystem/WBP_MainMenu"));
    if (!ensure(MainMenuWidgetBPClass.Class != nullptr)) return;
    MainMenuWidgetClass = MainMenuWidgetBPClass.Class;

    ConstructorHelpers::FClassFinder<UUserWidget> InGameMenuWidgetBPClass(TEXT("/Game/MenuSystem/WBP_InGameMenu"));
    if (!ensure(nullptr != InGameMenuWidgetBPClass.Class)) return;
    InGameMenuWidgetClass = InGameMenuWidgetBPClass.Class;
}

void UPuzzlePlatformsGameInstance::Init()
{
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (nullptr != Subsystem)
    {
        FName SubsystemName = Subsystem->GetSubsystemName();
        if (SubsystemName != "STEAM")
        {
            UE_LOG(LogTemp, Warning, TEXT("Not 'STEAM' subsystem ,but found subsystem %s"), *SubsystemName.ToString());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("'STEAM' OnlineSubsystem is active."));

            // #if WITH_STEAMWORKS // it is working with STEAM SDK
            // if (false == SteamAPI_IsSteamRunning())
            // {
            //     UE_LOG(LogTemp, Error, TEXT("Steam API is not running. Steam client may not be launched."));
            // }
            // #endif
        }
        
        SessionInterface = Subsystem->GetSessionInterface();

        if (true == SessionInterface.IsValid())
        {
            

            SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnCreateSessionComplete);
            SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnDestroyedSessionComplete);
            SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnFindSessionComplete);
            SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnJoinSessionComplete);
        }
    }

    // UE_LOG(LogTemp, Warning, TEXT("Found class %s"), *MainMenuWidgetClass->GetName());

    if (nullptr != GEngine)
    {
        GEngine->OnNetworkFailure().AddUObject(this, &UPuzzlePlatformsGameInstance::OnNetworkFailure);
    }
}

void UPuzzlePlatformsGameInstance::Host(const FText& HostName)
{
    TryCreateSession(HostName);
}

void UPuzzlePlatformsGameInstance::Join(uint32 Index)
{
    if (false == SessionInterface.IsValid()) return;
    if (false == SessionSearch.IsValid()) return;

    UE_LOG(LogTemp, Warning, TEXT("Join"));
    FNamedOnlineSession* ExistSession = SessionInterface->GetNamedSession(SESSION_NAME);
    if (nullptr != ExistSession)
    {
        UE_LOG(LogTemp, Warning, TEXT("Session already exists — Destroying before Join"));
        IsSessionJoining = true;
        SessionIndex = Index;
        SessionInterface->DestroySession(SESSION_NAME);
        return;
    }
     UE_LOG(LogTemp, Warning, TEXT("JoinSession"));
    SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[Index]);
}

void UPuzzlePlatformsGameInstance::FindSessions()
{
    SessionSearch = MakeShareable(new FOnlineSessionSearch());
    if (true == SessionInterface.IsValid())
    {
        if (true == SessionSearch.IsValid())
        {
            SessionSearch->QuerySettings.Set(SEARCH_KEYWORDS, FString("PuzzlePlatformSession"), EOnlineComparisonOp::Equals);
            SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
            SessionSearch->MaxSearchResults = 100;
            SessionSearch->bIsLanQuery = false;
            UE_LOG(LogTemp, Warning, TEXT("Starting find session"));
            SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
        }
    }
}

void UPuzzlePlatformsGameInstance::LoadMainMenu()
{
    if (APlayerController* PlayerController = GetFirstLocalPlayerController())
    {
        PlayerController->ClientTravel("/Game/MenuSystem/MainMenu", ETravelType::TRAVEL_Absolute);
    }
}

void UPuzzlePlatformsGameInstance::LoadMenuWidget()
{
    if (nullptr == MainMenuWidgetClass) return;
    MainMenu = CreateWidget<UMainMenu>(this, MainMenuWidgetClass, FName("MainMenu"));

    if (!ensure(nullptr != MainMenu)) return;
    MainMenu->Setup();
    MainMenu->SetMenuInterface(this);
}

void UPuzzlePlatformsGameInstance::LoadInGameMenu()
{
    if (nullptr == InGameMenuWidgetClass) return;
    InGameMenu = CreateWidget<UInGameMenu>(this, InGameMenuWidgetClass, FName("InGameMenu"));

    if (!ensure(nullptr != InGameMenu)) return;
    InGameMenu->Setup();
    InGameMenu->SetMenuInterface(this);
}

void UPuzzlePlatformsGameInstance::TryCreateSession(const FText& HostName)
{
    OpHostName = HostName.ToString();
    FNamedOnlineSession* ExistSession = SessionInterface->GetNamedSession(SESSION_NAME);
    if (nullptr != ExistSession)
    {
        IsSessionCreating = true;
        SessionInterface->DestroySession(SESSION_NAME);
        return;
    }

    CreateSession();
}

void UPuzzlePlatformsGameInstance::StartSession()
{
    if (SessionInterface.IsValid())
    {
        SessionInterface->StartSession(SESSION_NAME);
    }
}

void UPuzzlePlatformsGameInstance::OnCreateSessionComplete(FName SessionName, bool Success)
{
    if (false == Success)
    {
        UE_LOG(LogTemp, Warning, TEXT("Could not create session"));
        return;
    }
    UE_LOG(LogTemp, Warning, TEXT("Complete Create session"));
    UEngine* Engine = GetEngine();
    if (!ensure(nullptr != Engine)) return;
    
    Engine->AddOnScreenDebugMessage(-1, 2, FColor::Green, TEXT("Hosting"));

    UWorld* World = GetWorld();
    if (!ensure(nullptr != World)) return;

    if (true == World->ServerTravel("/Game/Maps/Lobby?listen"))
    {
        // if (ensure(MainMenu != nullptr))
        // {
        //     MainMenu->Teardown();
        // }
    }
}

void UPuzzlePlatformsGameInstance::OnDestroyedSessionComplete(FName SessionName, bool Success)
{
    if (false == Success)
    {
        IsSessionJoining = false;
        IsSessionCreating = false;
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("Destroied Session"));
    if (true == IsSessionCreating)
    {
        CreateSession();
    }
    else if (true == IsSessionJoining && true == SessionIndex.IsSet() && true == SessionSearch.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("JoinSession"));
        SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[SessionIndex.GetValue()]);
    }
    
    IsSessionJoining = false;
    IsSessionCreating = false;
}

void UPuzzlePlatformsGameInstance::OnFindSessionComplete(bool Success)
{
    if (true == Success && true == SessionSearch.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("Completed Find Session"));
        
        for (FOnlineSessionSearchResult Result : SessionSearch->SearchResults)
        {
            UE_LOG(LogTemp, Warning, TEXT("SessionId: %s"), *Result.GetSessionIdStr());
        }

        RefreshServerList();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed Find Session"));
        RefreshServerList();
    }
}

void UPuzzlePlatformsGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    if (false == SessionInterface.IsValid()) return;

    FString Address;
    if (false == SessionInterface->GetResolvedConnectString(SessionName, Address))
    {
        UE_LOG(LogTemp, Warning, TEXT("Could not get connect string."));
        return;
    }

    UEngine* Engine = GetEngine();
    if (!ensure(Engine != nullptr)) return;
    
    Engine->AddOnScreenDebugMessage(-1, 2, FColor::Green, FString::Printf(TEXT("Joining %s"), *Address));

    if (APlayerController* PlayerController = GetFirstLocalPlayerController())
    {
        UE_LOG(LogTemp, Warning, TEXT("ClientTraveling"));
        PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
    }
}

void UPuzzlePlatformsGameInstance::OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type NetworkFailureType, const FString& ErrorString)
{
    LoadMainMenu();
}


void UPuzzlePlatformsGameInstance::CreateSession()
{
    if (false == SessionInterface.IsValid())
    {
        return;
    }
    FOnlineSessionSettings SessionSettings;

    if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
    {
        SessionSettings.bIsLANMatch = true;
    }
    else
    {
        SessionSettings.bIsLANMatch = false;
    }

    SessionSettings.bAllowJoinInProgress = true;
    SessionSettings.bAllowInvites = true;
    SessionSettings.NumPublicConnections = 5;
    SessionSettings.bShouldAdvertise = true;
    SessionSettings.bUsesPresence = true;
    SessionSettings.bAllowJoinViaPresence = true;
    SessionSettings.bUseLobbiesIfAvailable = true; // before bUsesPresence is available option for create lobby, but now need bUseLobbiesIfAvailable option.
    SessionSettings.Set(SEARCH_KEYWORDS, FString("PuzzlePlatformSession"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

    if (true == OpHostName.IsSet())
    {
        SessionSettings.Set(HOSTNAME_SETTING_KEY, OpHostName.GetValue(), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
    }
    SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
}

void UPuzzlePlatformsGameInstance::RefreshServerList()
{
    if (!ensure(MainMenu != nullptr)) return;

    TArray<FServerData> ServerDatas;
    for (FOnlineSessionSearchResult Result : SessionSearch->SearchResults)
    {
        if (true == Result.IsValid())
        {
            FServerData Data;
            // Data.Name = Result.GetSessionIdStr();
            Data.MaxPlayers = Result.Session.SessionSettings.NumPublicConnections;
            Data.CurrentPlayers = Data.MaxPlayers - Result.Session.NumOpenPublicConnections;
            Data.HostUsername = Result.Session.OwningUserName;
            // FString HostName;
            if (Result.Session.SessionSettings.Get(HOSTNAME_SETTING_KEY, Data.Name))
            {
                UE_LOG(LogTemp, Warning, TEXT("Data found in Setting : %s"), *Data.Name);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("didn't found data"));
            }
            
            ServerDatas.Add(Data);
            //UE_LOG(LogTemp, Warning, TEXT("SessionId: %s"), *Result.GetSessionIdStr());
        }
    }
    // ServerNames.Add(TEXT("Test Server 1"));
    // ServerNames.Add(TEXT("Test Server 2"));
    // ServerNames.Add(TEXT("Test Server 3"));
    // ServerNames.Add(TEXT("Test Server 4"));
    
    MainMenu->SetServerList(ServerDatas);
}
