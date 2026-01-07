// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/StreamableManager.h"
#include "UObject/PrimaryAssetId.h"

#include "ItemAssetSubsystem.generated.h"

class UItemDef;

typedef TMap<FPrimaryAssetId, UItemDef*> FItemDefMap;

DECLARE_DELEGATE_OneParam(FOnItemDefLoaded, UItemDef*);
DECLARE_DELEGATE_TwoParams(FOnItemDefsLoaded, const FItemDefMap&, bool);

struct FItemDefsBatchContext : public TSharedFromThis<FItemDefsBatchContext>
{
	TWeakObjectPtr<UObject> Owner;
	TWeakObjectPtr<UItemAssetSubsystem> AM;
	TSharedPtr<FStreamableHandle> Handle;
	TArray<FPrimaryAssetId> AllIds;
	TArray<FPrimaryAssetId> RequestIds;
	TMap<FPrimaryAssetId, UItemDef*> LoadedDefs;
	FOnItemDefsLoaded Callback;
};
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
		const FPrimaryAssetId& InId,
		const TArray<FName>& Bundles,
		TWeakObjectPtr<UObject> Owner,
		FOnItemDefLoaded Callback);

	TSharedPtr<FStreamableHandle> LoadItemDefAsync(
		const TArray<FPrimaryAssetId>& InIds,
		const TArray<FName>& Bundles,
		TWeakObjectPtr<UObject> Owner,
		FOnItemDefsLoaded Callback);

	void CancelByOwner(UObject* Owner);

	void TrackHandle(TSharedPtr<FStreamableHandle> Handle)
	{
		if (Handle.IsValid())
		{
			ActiveHandles.Add(Handle);
		}
	}

	void ReleaseHandle(TSharedPtr<FStreamableHandle> Handle)
	{
		if (Handle.IsValid())
		{
			ActiveHandles.Remove(Handle);
		}
	}

private:
	TMap<FPrimaryAssetId, TWeakObjectPtr<UItemDef>> Cache;
	TMap<FPrimaryAssetId, TSharedPtr<FStreamableHandle>> InFlight;
	TMap<FPrimaryAssetId, TArray<TPair<TWeakObjectPtr<UObject>, FOnItemDefLoaded>>> Waiters;
	TSet<TSharedPtr<FStreamableHandle>> ActiveHandles;

	void OnLoaded(const FPrimaryAssetId Id);
	void OnLoaded(TSharedRef<FItemDefsBatchContext> BatchContext);
};
