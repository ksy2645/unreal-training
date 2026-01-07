// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItemPusher.h"

#include "Engine/AssetManager.h"

#include "Components/Button.h"
#include "Components/SpinBox.h"
#include "Components/ComboBoxString.h"

#include "Systems/ItemAssetSubsystem.h"

#include "UItemTileEntry.h"

void UInventoryItemPusherWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	UE_LOG(LogTemp, Warning, TEXT("UInventoryItemPusherWidget::NativeOnInitialized"));
	//if (AddItemButton)
	//{
	//	AddItemButton->OnClicked.AddUniqueDynamic(this, &UInventoryItemPusherWidget::HandlePushClicked);
	//}

	//if (ItemComboBox)
	//{
	//	ItemComboBox->OnSelectionChanged.AddUniqueDynamic(this, &UInventoryItemPusherWidget::HandleItemSelectionChanged);
	//}
}

void UInventoryItemPusherWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (AddItemButton)
	{
		AddItemButton->OnClicked.AddUniqueDynamic(this, &UInventoryItemPusherWidget::HandlePushClicked);
	}

	if (ItemComboBox)
	{
		ItemComboBox->OnSelectionChanged.AddUniqueDynamic(this, &UInventoryItemPusherWidget::HandleItemSelectionChanged);
	}

	RefreshItemIdList();
}

void UInventoryItemPusherWidget::RefreshItemIdList()
{
	OrderedItemIds.Reset();
	CachedItemIds.Reset();
	if (ItemComboBox)
	{
		ItemComboBox->ClearOptions();
	}

	TArray<FPrimaryAssetId> OutIds;
	UAssetManager& AM = UAssetManager::Get();

	AM.GetPrimaryAssetIdList(ItemPrimaryType, OutIds);

	OutIds.Sort([](const FPrimaryAssetId& A, const FPrimaryAssetId& B)
		{
			return A.PrimaryAssetName.LexicalLess(B.PrimaryAssetName);
		});

	for (const FPrimaryAssetId& Id : OutIds)
	{
		OrderedItemIds.Add(Id);
	}

	if (UGameInstance* GI = GetGameInstance())
	{
		for (const FPrimaryAssetId& Id : OutIds)
		{
			auto* Items = GI->GetSubsystem<UItemAssetSubsystem>();
			PendingItemDefsToLoad++;
			// 비동기적으로 아이템이 로드 되기 때문에 순서가 보장되지 않는다.
			Items->LoadItemDefAsync(Id, { FName("UI") }, this,
				FOnItemDefLoaded::CreateLambda([this, Id](UItemDef* Def)
				{
					if (!IsValid(this))
					{
						return;
					}

					CachedItemIds.Add(Id, Def);

					PendingItemDefsToLoad--;
					if (PendingItemDefsToLoad == 0)
					{
						UE_LOG(LogTemp, Warning, TEXT("UInventoryItemPusherWidget::RefreshItemIdList: All ItemDefs Loaded"));
						FinalizeItemList();
					}
				}));
		}
	}

	if (PendingItemDefsToLoad == 0)
	{
		FinalizeItemList();
	}
}

void UInventoryItemPusherWidget::FinalizeItemList()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		for (const FPrimaryAssetId& Id : OrderedItemIds)
		{
			const UItemDef* Def = CachedItemIds.FindRef(Id).Get();
			if (ItemComboBox && Def)
			{
				const FString Label = Def->DisplayName.ToString();
				ItemComboBox->AddOption(Label);
			}
		}
	}
	else
	{
		for (const FPrimaryAssetId& Id : OrderedItemIds)
		{
			if (ItemComboBox)
			{
				const FString Label = Id.PrimaryAssetName.ToString();
				ItemComboBox->AddOption(Label);
			}
		}
	}

	if (ItemComboBox && ItemComboBox->GetOptionCount() > 0)
	{
		ItemComboBox->SetSelectedIndex(0);
	}
}

void UInventoryItemPusherWidget::HandleItemSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	UE_LOG(LogTemp, Warning, TEXT("UInventoryItemPusherWidget::HandleItemSelectionChanged: %s"), *SelectedItem);
	const int32 Index = ItemComboBox->GetSelectedIndex();
	FPrimaryAssetId Id;
	if (OrderedItemIds.IsValidIndex(Index))
	{
		Id = OrderedItemIds[Index];
	}

	if (!Id.IsValid())
	{
		return;
	}

	if (SelectedItemEntry)
	{
		SelectedItemEntry->SetItemId(Id);
	}

	if (AddItemCountComboBox)
	{
		AddItemCountComboBox->ClearOptions();
		const UItemDef* Def = CachedItemIds.FindRef(Id).Get();
		if (Def)
		{
			for (int32 ii = 0; ii < Def->MaxStack; ii++)
			{
				AddItemCountComboBox->AddOption(FString::FromInt(ii + 1));
			}
		}
		else
		{
			AddItemCountComboBox->AddOption("1");
		}
	}
}

void UInventoryItemPusherWidget::HandlePushClicked()
{
	if (!ItemComboBox || CachedItemIds.Num() == 0)
	{
		return;
	}

	const int32 Index = ItemComboBox->GetSelectedIndex();
	if (!OrderedItemIds.IsValidIndex(Index)) return;

	FItemStack Stack;
	Stack.ItemId = OrderedItemIds[Index];

	int32 Count = 1;
	if (AddItemCountComboBox)
	{
		Count = FCString::Atoi(*AddItemCountComboBox->GetSelectedOption());
	}
	Stack.Count = Count;

	OnPushRequested.Broadcast(Stack);
}

