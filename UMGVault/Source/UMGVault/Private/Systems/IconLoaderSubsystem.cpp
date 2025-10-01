// Fill out your copyright notice in the Description page of Project Settings.


#include "Systems/IconLoaderSubsystem.h"
#include "Engine/AssetManager.h"


TSharedPtr<FStreamableHandle> UIconLoaderSubsystem::RequestIconAsync(
	const TSoftObjectPtr<UTexture2D>& IconRef,
	TWeakObjectPtr<UObject> Owner,
	FOnReady OnReady)
{
	if (IconRef.IsNull())
	{
		if (OnReady)
		{
			OnReady(nullptr);
			return {};
		}
	}
	
	const FSoftObjectPath IconPath = IconRef.ToSoftObjectPath();

	if (UTexture2D* Cached = Cache.FindRef(IconPath).Get())
	{
		if (OnReady)
		{
			OnReady(Cached);
			return {};
		}
	}

	Waiters.FindOrAdd(IconPath).Add({ Owner, MoveTemp(OnReady) });

	if (InFlight.Contains(IconPath))
	{
		return InFlight[IconPath];
	}

	auto& SM = UAssetManager::GetStreamableManager();
	TSharedPtr<FStreamableHandle> Handle = SM.RequestAsyncLoad(
		IconPath, FStreamableDelegate::CreateUObject(this, &UIconLoaderSubsystem::OnLoaded, IconPath));
	InFlight.Add(IconPath, Handle);
	return Handle;
}

void UIconLoaderSubsystem::OnLoaded(const FSoftObjectPath IconPath)
{
	TSharedPtr<FStreamableHandle> Handle = InFlight.FindRef(IconPath);
	InFlight.Remove(IconPath);

	UTexture2D* Texture = Cast<UTexture2D>(IconPath.ResolveObject());
	if (!Texture && Handle.IsValid())
	{
		Texture = Cast<UTexture2D>(Handle->GetLoadedAsset());
	}

	if (Texture)
	{
		Cache.Add(IconPath, Texture);
	}

	if (auto* Arr = Waiters.Find(IconPath))
	{
		for (auto& Pair : *Arr)
		{
			if (Pair.Key.IsValid() && Pair.Value)
			{
				Pair.Value(Texture);
			}
		}
		Waiters.Remove(IconPath);
	}
}

void UIconLoaderSubsystem::CancelByOwner(UObject* Owner)
{
	for (auto& KV : Waiters)
	{
		KV.Value.RemoveAll([Owner](const auto& P)
			{
				return P.Key.Get() == Owner;
			});
	}
}