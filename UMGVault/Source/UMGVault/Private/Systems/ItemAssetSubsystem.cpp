// Fill out your copyright notice in the Description page of Project Settings.


#include "Systems/ItemAssetSubsystem.h"
#include "Engine/AssetManager.h"
#include "Data/Items/ItemDef.h"
#include "Systems/IconLoaderSubsystem.h"


TSharedPtr<FStreamableHandle> UItemAssetSubsystem::LoadItemDefAsync(const FPrimaryAssetId& InId, const TArray<FName>& Bundles, TWeakObjectPtr<UObject> Owner, FOnItemDefLoaded Callback)
{
	// 이미 캐시된 경우 바로 콜백 실행
	if (UItemDef* Cached = Cache.FindRef(InId).Get())
	{
		if (Callback.IsBound())
		{
			Callback.Execute(Cached);
			return {};
		}
	}

	// 대기열에 추가
	Waiters.FindOrAdd(InId).Add({ Owner, MoveTemp(Callback) });

	// 이미 로드 중인 경우 핸들 반환
	if (TSharedPtr<FStreamableHandle>* Found = InFlight.Find(InId))
	{
		return *Found;
	}

	UAssetManager& AM = UAssetManager::Get();
	TSharedPtr<FStreamableHandle> Handle = AM.LoadPrimaryAsset(
		InId, Bundles,
		FStreamableDelegate::CreateUObject(this, &UItemAssetSubsystem::OnLoaded, InId));

	InFlight.Add(InId, Handle);
	TrackHandle(Handle);

	return Handle;
}

TSharedPtr<FStreamableHandle> UItemAssetSubsystem::LoadItemDefAsync(const TArray<FPrimaryAssetId>& InIds, const TArray<FName>& Bundles, TWeakObjectPtr<UObject> Owner, FOnItemDefsLoaded Callback)
{
	TArray<FPrimaryAssetId> AllIds;
	AllIds.Reserve(InIds.Num());
	for (const FPrimaryAssetId& Id : InIds)
	{
		if (Id.IsValid())
		{
			AllIds.Add(Id);
		}
	}

	if (AllIds.Num() == 0)
	{
		if (Callback.IsBound())
		{
			Callback.Execute(TMap<FPrimaryAssetId, UItemDef*>(), false);
		}
		return {};
	}

	TSharedRef<FItemDefsBatchContext> BatchContext = MakeShared<FItemDefsBatchContext>();
	BatchContext->Owner = Owner;
	BatchContext->AM = this;
	BatchContext->AllIds = AllIds;
	BatchContext->Callback = Callback;
	BatchContext->RequestIds.Reserve(AllIds.Num());

	for (const FPrimaryAssetId& Id : AllIds)
	{
		if (UItemDef* Cached = Cache.FindRef(Id).Get())
		{
			BatchContext->LoadedDefs.Add(Id, Cached);
		}
		else
		{
			BatchContext->LoadedDefs.Add(Id, nullptr);
			BatchContext->RequestIds.Add(Id);
		}
	}

	if (BatchContext->RequestIds.Num() == 0)
	{
		Callback.ExecuteIfBound(BatchContext->LoadedDefs, true);
		return {};
	}

	UAssetManager& AM = UAssetManager::Get();

	TSharedPtr<FStreamableHandle> Handle = AM.LoadPrimaryAssets(
		BatchContext->RequestIds, Bundles,
		FStreamableDelegate::CreateUObject(this, &UItemAssetSubsystem::OnLoaded, BatchContext));

	//TSharedPtr<FStreamableHandle> Handle = AM.LoadPrimaryAssets(
	//	BatchContext->RequestIds, Bundles,
	//	FStreamableDelegate::CreateWeakLambda(this, [BatchContext]()
	//		{
	//			if (!BatchContext->Owner.IsValid())
	//				return;

	//			UItemAssetSubsystem* Self = BatchContext->Owner.Get();

	//			for (FPrimaryAssetId& TempId : BatchContext->RequestIds)
	//			{
	//				UItemDef* Def = UAssetManager::Get().GetPrimaryAssetObject<UItemDef>(TempId);
	//				if (Def)
	//				{
	//					Self->Cache.Add(TempId, Def);
	//					BatchContext->LoadedDefs[TempId] = Def;
	//				}
	//			}

	//			Self->ReleaseHandle(BatchContext->Handle);
	//			BatchContext->Callback.ExecuteIfBound(BatchContext->LoadedDefs);

	//		}
	//	));

	BatchContext->Handle = Handle;
	
	if (!Handle.IsValid())
	{
		return {};
	}

	TrackHandle(Handle);

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

	UItemDef* Def = UAssetManager::Get().GetPrimaryAssetObject<UItemDef>(Id);
	if (Def)
	{
		Cache.Add(Id, Def);
	}

	if (TArray<TPair<TWeakObjectPtr<UObject>, FOnItemDefLoaded>>* Arr = Waiters.Find(Id))
	{
		for (auto& Pair : *Arr)
		{
			if (Pair.Key.IsValid() && Pair.Value.IsBound())
			{
				Pair.Value.Execute(Def);
			}
		}
		Waiters.Remove(Id);
	}
}

void UItemAssetSubsystem::OnLoaded(TSharedRef<FItemDefsBatchContext> BatchContext)
{
	if(!BatchContext->AM.IsValid() || !BatchContext->Owner.IsValid())
	{
		ReleaseHandle(BatchContext->Handle);
		return;
	}

	bool bAllLoaded = true;
	for (FPrimaryAssetId& Id : BatchContext->RequestIds)
	{
		UItemDef* Def = UAssetManager::Get().GetPrimaryAssetObject<UItemDef>(Id);
		if (Def)
		{
			Cache.Add(Id, Def);
			BatchContext->LoadedDefs[Id] = Def;
		}
	}

	//for (FPrimaryAssetId& Id : BatchContext->AllIds)
	//{
	//	if (BatchContext->LoadedDefs[Id] == nullptr)
	//	{
	//		bAllLoaded = false;
	//		break;
	//	}
	//}

	ReleaseHandle(BatchContext->Handle);
	BatchContext->Callback.ExecuteIfBound(BatchContext->LoadedDefs, bAllLoaded);

}
