// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/StreamableManager.h"

#include "IconLoaderSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class UMGVAULT_API UIconLoaderSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	using FOnReady = TFunction<void(UTexture2D*)>;

	TSharedPtr<FStreamableHandle> RequestIconAsync(
		const TSoftObjectPtr<UTexture2D>& IconRef,
		TWeakObjectPtr<UObject> Owner,
		FOnReady OnReady);

	void CancelByOwner(UObject* Owner);

private:
	TMap<FSoftObjectPath, TWeakObjectPtr<UTexture2D>> Cache;
	TMap<FSoftObjectPath, TSharedPtr<FStreamableHandle>> InFlight;
	TMap<FSoftObjectPath, TArray<TPair<TWeakObjectPtr<UObject>, FOnReady>>> Waiters;

	void OnLoaded(const FSoftObjectPath IconPath);
};
