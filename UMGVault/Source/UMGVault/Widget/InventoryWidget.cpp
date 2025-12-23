// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"

#include "Components/ComboBox.h"
#include "Components/ComboBoxString.h"
#include "Components/TileView.h"
#include "Components/EditableTextBox.h"

#include "Features/Inventory/InventoryDataSource.h"



void UInventoryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	DataSource = NewObject<UInventoryDataSource>(this);

	if (ComboBox_Sort)
	{
		ComboBox_Sort->OnSelectionChanged.AddUniqueDynamic(this, &UInventoryWidget::OnSortSelectionChanged);
	}

	if (EditableText_Search)
	{
		EditableText_Search->OnTextChanged.AddUniqueDynamic(this, &UInventoryWidget::OnSearchTextChanged);
	}
}

void UInventoryWidget::OnSortSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	UE_LOG(LogTemp, Log, TEXT("UInventoryWidget::OnSortSelectionChanged: %s"), *SelectedItem);
	RebuildItemList();
}

void UInventoryWidget::OnSearchTextChanged(const FText& Text)
{
	RebuildItemList();
}

void UInventoryWidget::SetStacks(const TArray<FItemStack>& InStacks)
{
	DataSource->SetStacks(InStacks);
	RebuildItemList();
}

void UInventoryWidget::RebuildItemList()
{
	CurrentFilter.Query = EditableText_Search ? EditableText_Search->GetText().ToString() : TEXT("");
	CurrentFilter.Rarity.Reset();

	const FString Selected = ComboBox_Sort ? ComboBox_Sort->GetSelectedOption() : TEXT("");
	if (Selected == TEXT("Name ↑")) CurrentSort = EInventorySort::NameAsc;
	else if (Selected == TEXT("Name ↓")) CurrentSort = EInventorySort::NameDesc;
	else if (Selected == TEXT("Rarity ↑")) CurrentSort = EInventorySort::RarityAsc;
	else if (Selected == TEXT("Rarity ↓")) CurrentSort = EInventorySort::RarityDesc;
	else if (Selected == TEXT("Price ↑")) CurrentSort = EInventorySort::PriceAsc;
	else if (Selected == TEXT("Price ↓")) CurrentSort = EInventorySort::PriceDesc;
	else CurrentSort = EInventorySort::NameAsc;

	static_assert(static_cast<int8>(EInventorySort::COUNT) == 6, "EInventorySort가 추가 되었다면 여기도 작업 해주세요.");

	TArray<UObject*> ListItems;
	DataSource->Build(CurrentFilter, CurrentSort, ListItems, [this, ListItems = MoveTemp(ListItems)] {
			if (!IsValid(this) || !TileView_Items)
				return;
			TileView_Items->ClearListItems();
			for (UObject* Item : ListItems)
			{
				TileView_Items->AddItem(Item);
			}
		});
}