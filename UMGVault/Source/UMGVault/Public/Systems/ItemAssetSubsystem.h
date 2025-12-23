// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/StreamableManager.h"
#include "UObject/PrimaryAssetId.h"

#include "ItemAssetSubsystem.generated.h"

class UItemDef;

/**
 * 
 */
UCLASS()
class UMGVAULT_API UItemAssetSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	using FOnDefReady = TFunction<void(UItemDef*)>;
	using FOnDefsReady = TFunction<void(const TMap<FPrimaryAssetId, UItemDef*>&)>;

	TSharedPtr<FStreamableHandle> LoadItemDefAsync(
		const FPrimaryAssetId& Id,
		const TArray<FName>& Bundles,
		TWeakObjectPtr<UObject> Owner,
		FOnDefReady OnReady);

	TSharedPtr<FStreamableHandle> LoadItemDefAsync(
		const TArray<FPrimaryAssetId>& Ids,
		const TArray<FName>& Bundles,
		TWeakObjectPtr<UObject> Owner,
		FOnDefsReady OnReady);

	void CancelByOwner(UObject* Owner);

private:
	TMap<FPrimaryAssetId, TWeakObjectPtr<UItemDef>> Cache;
	TMap<FPrimaryAssetId, TSharedPtr<FStreamableHandle>> InFlight;
	TMap<FPrimaryAssetId, TArray<TPair<TWeakObjectPtr<UObject>, FOnDefReady>>> Waiters;

	void OnLoaded(const FPrimaryAssetId Id);
};
