// Fill out your copyright notice in the Description page of Project Settings.


#include "Systems/ItemAssetSubsystem.h"
#include "Engine/AssetManager.h"
#include "Data/Items/ItemDef.h"
#include "Systems/IconLoaderSubsystem.h"



TSharedPtr<FStreamableHandle> UItemAssetSubsystem::LoadItemDefAsync(const FPrimaryAssetId& Id, const TArray<FName>& Bundles, TWeakObjectPtr<UObject> Owner, FOnDefReady OnReady)
{
	if (UItemDef* Cached = Cache.FindRef(Id).Get())
	{
		if (OnReady)
		{
			OnReady(Cached);
			return {};
		}
	}

	Waiters.FindOrAdd(Id).Add({ Owner, MoveTemp(OnReady) });

	if (TSharedPtr<FStreamableHandle>* Found = InFlight.Find(Id))
	{
		return *Found;
	}

	UAssetManager& AM = UAssetManager::Get();
	TSharedPtr<FStreamableHandle> Handle = AM.LoadPrimaryAsset(
		Id, Bundles,
		FStreamableDelegate::CreateUObject(this, &UItemAssetSubsystem::OnLoaded, Id));
	InFlight.Add(Id, AM.GetPrimaryAssetHandle(Id, true));

	return Handle;
}

void UItemAssetSubsystem::CancelByOwner(UObject* Owner)
{
	for (auto& KV : Waiters)
	{
		KV.Value.RemoveAll([Owner](const auto& P)
			{
				return P.Key.Get() == Owner;
			}
		);
	}
}

void UItemAssetSubsystem::OnLoaded(const FPrimaryAssetId Id)
{
	TSharedPtr<FStreamableHandle> Handle = InFlight.FindRef(Id);
	InFlight.Remove(Id);

	UObject* Obj = UAssetManager::Get().GetPrimaryAssetObject(Id);
	UItemDef* Def = Cast<UItemDef>(Obj);
	if (Def)
	{
		Cache.Add(Id, Def);
	}

	if (TArray<TPair<TWeakObjectPtr<UObject>, FOnDefReady>>* Arr = Waiters.Find(Id))
	{
		for (auto& Pair : *Arr)
		{
			if (Pair.Key.IsValid() && Pair.Value)
			{
				Pair.Value(Def);
			}
		}
		Waiters.Remove(Id);
	}
}
