// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MenuSystem/MenuInterface.h"
#include "OnlineSubsystem.h"
#include "Interfaces\OnlineSessionInterface.h"


#include "PuzzlePlatformsGameInstance.generated.h"
class UUserWidget;

/**
 * 
 */
UCLASS()
class PUZZLEPLATFORMS_API UPuzzlePlatformsGameInstance : public UGameInstance, public IMenuInterface
{
	GENERATED_BODY()

public:
	UPuzzlePlatformsGameInstance(const FObjectInitializer& ObjectInitializer);

	virtual void Init() override;

	UFUNCTION(Exec)
	virtual void Host(const FText& HostName) override;

	UFUNCTION(Exec)
	virtual void Join(uint32 Index) override;

	UFUNCTION()
	virtual void FindSessions() override;

	UFUNCTION(Exec, BlueprintCallable)
	virtual void LoadMainMenu() override;

	UFUNCTION(Exec, BlueprintCallable)
	void LoadMenuWidget();

	UFUNCTION(Exec, BlueprintCallable)
	void LoadInGameMenu();

	UFUNCTION(Exec)
	void TryCreateSession(const FText& HostName);

	void StartSession();

private:
	TSubclassOf<UUserWidget> MainMenuWidgetClass;
	TSubclassOf<UUserWidget> InGameMenuWidgetClass;

	class UMainMenu* MainMenu;
	class UInGameMenu* InGameMenu;

	UUserWidget* CurrentUserWidget;
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	bool IsSessionCreating = false;
	bool IsSessionJoining = false;
	TOptional<uint32> SessionIndex;
	TOptional<FString> OpHostName;

	void OnCreateSessionComplete(FName SessionName, bool Success);
	void OnDestroyedSessionComplete(FName SessionName, bool Success);
	void OnFindSessionComplete(bool Success);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type NetworkFailureType, const FString& ErrorString);
	
	void CreateSession();
	void RefreshServerList();
};
