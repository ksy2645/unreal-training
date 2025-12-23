// Fill out your copyright notice in the Description page of Project Settings.


#include "Features/Inventory/InventoryDataSource.h"
#include "Engine/AssetManager.h"
#include "Data/Items/ItemDef.h"

static int32 RarityRank(EItemRarity Rarity); // 전방 선언

void UInventoryDataSource::Build(const FInventoryFilter& Filter, EInventorySort Sort, TArray<UObject*>& OutItems, TFunction<void()> OnAsyncReady)
{
	// 복사본 만들어서 필터/정렬 적용
	TArray<FItemStack> View = Stacks;

	// Def 메타가 필요한 필터/정렬이 아니면 바로 결과 반환
	if (!NeedDefs(Filter, Sort))
	{
		OutItems.Reset(View.Num());
		for (const FItemStack& S : View)
		{
			UInventoryEntryObject* Entry = NewObject<UInventoryEntryObject>(this);
			Entry->ItemId = S.ItemId;
			Entry->Count = S.Count;
			OutItems.Add(Entry);
		}

		if (OnAsyncReady)
			OnAsyncReady();

		return;
	}

	// Def 메타가 필요한 필터/정렬이면, 우선 Def 로드
	TArray<FPrimaryAssetId> Ids;
	Ids.Reserve(View.Num());

	for (const FItemStack& S : View)
	{
		if (S.ItemId.IsValid())
			Ids.AddUnique(S.ItemId);
	}

	if (!UAssetManager::IsInitialized() || Ids.Num() == 0)
	{
		// AssetManager가 없거나 로드할 Def가 없으면 바로 결과 반환, ex) 디자인타임
		OutItems.Reset(View.Num());
		for (const FItemStack& S : View)
		{
			UInventoryEntryObject* Entry = NewObject<UInventoryEntryObject>(this);
			Entry->ItemId = S.ItemId;
			Entry->Count = S.Count;
			OutItems.Add(Entry);
		}
		if (OnAsyncReady)
			OnAsyncReady();
		return;
	}

	// Def 로드
	UAssetManager& AM = UAssetManager::Get();
	TSharedPtr<FStreamableHandle> Handle = AM.LoadPrimaryAssets(
		Ids, {FName("UI")},
		FStreamableDelegate::CreateWeakLambda(this, [this, Filter, Sort, View = MoveTemp(View), OnAsyncReady, &OutItems]()
			{
				TArray<FItemStack> Sorted = View;
				const_cast<UInventoryDataSource*>(this)->FilterAndSort_UsingDefs(Filter, Sort, Sorted);

				OutItems.Reset(Sorted.Num());
				for (const FItemStack& S : Sorted)
				{
					UInventoryEntryObject* Row = NewObject<UInventoryEntryObject>(this);
					Row->ItemId = S.ItemId;
					Row->Count = S.Count;
					OutItems.Add(Row);
				}
				if (OnAsyncReady)
					OnAsyncReady();
			})
	);
}

void UInventoryDataSource::AddItemStack(const FItemStack& InStack)
{
	if (!InStack.ItemId.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryDataSource::AddItemStack: Invalid ItemId"));
		return;
	}


	InStack.ItemId;

}

bool UInventoryDataSource::NeedDefs(const FInventoryFilter& Filter, EInventorySort Sort)
{
	if (Filter.Rarity.Num() > 0 || !Filter.Query.IsEmpty())
		return true;

	switch (Sort)
	{
	case EInventorySort::RarityAsc:
	case EInventorySort::RarityDesc:
	case EInventorySort::NameAsc:
	case EInventorySort::NameDesc:
	case EInventorySort::PriceAsc:
	case EInventorySort::PriceDesc:
		return true;
	default: return false;
	}

	static_assert(static_cast<uint8>(EInventorySort::COUNT) == 6, "EInventorySort 변경 시 여기도 수정 필요");
}

void UInventoryDataSource::FilterAndSort_UsingDefs(const FInventoryFilter& Filter, EInventorySort Sort, TArray<FItemStack>& InOut)
{
	UAssetManager& AM = UAssetManager::Get();

	InOut = InOut.FilterByPredicate([&](const FItemStack& S)
		{
			if (!S.ItemId.IsValid())
				return false;

			UItemDef* Def = AM.GetPrimaryAssetObject<UItemDef>(S.ItemId);
			if (!Def)
				return false;

			if (Filter.Rarity.Num() > 0 && !Filter.Rarity.Contains(Def->Rarity))
				return false;

			if (!Filter.Query.IsEmpty())
			{
				const FString Name = Def->DisplayName.ToString();
				if (!Name.Contains(Filter.Query, ESearchCase::IgnoreCase))
					return false;
			}
			return true;
		});

	InOut.StableSort([&](const FItemStack& A, const FItemStack& B)
		{
			UItemDef* DefA = AM.GetPrimaryAssetObject<UItemDef>(A.ItemId);
			UItemDef* DefB = AM.GetPrimaryAssetObject<UItemDef>(B.ItemId);

			if (!DefA || !DefB)
				return false;

			switch (Sort)
			{
				case EInventorySort::RarityAsc:
					return RarityRank(DefA->Rarity) < RarityRank(DefB->Rarity);
				case EInventorySort::RarityDesc:
					return RarityRank(DefA->Rarity) > RarityRank(DefB->Rarity);
				case EInventorySort::NameAsc:
					return DefA->DisplayName.ToString() < DefB->DisplayName.ToString();
				case EInventorySort::NameDesc:
					return DefA->DisplayName.ToString() > DefB->DisplayName.ToString();
				case EInventorySort::PriceAsc:
					return DefA->Price < DefB->Price;
				case EInventorySort::PriceDesc:
					return DefA->Price > DefB->Price;
				default:
					return false;
			}
		});

	static_assert(static_cast<uint8>(EInventorySort::COUNT) == 6, "EInventorySort 변경 시 여기도 수정 필요");
}

static int32 RarityRank(EItemRarity Rarity)
{
	switch (Rarity)
	{
	case EItemRarity::Common:    return 0;
	case EItemRarity::Uncommon:  return 1;
	case EItemRarity::Rare:      return 2;
	case EItemRarity::Epic:      return 3;
	case EItemRarity::Legendary: return 4;
	case EItemRarity::Mythic:    return 5;
	case EItemRarity::Ancient:   return 6;
	default:                     return -1; // Undefined
	}

	static_assert(static_cast<uint8>(EItemRarity::COUNT) == 7, "EItemRarity 변경 시 여기도 수정 필요");
}