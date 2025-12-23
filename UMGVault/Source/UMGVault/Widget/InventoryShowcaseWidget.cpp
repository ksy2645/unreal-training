// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryShowcaseWidget.h"

#include "InventoryItemPusher.h"
#include "InventoryWidget.h"

void UInventoryShowcaseWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (InventoryItemPusher)
	{
		InventoryItemPusher->OnPushRequested.AddUniqueDynamic(this, &UInventoryShowcaseWidget::HandlePushRequested);
	}

	//TempItemStacks.Reset();
	ApplyInventoryWidget();
}

void UInventoryShowcaseWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UInventoryShowcaseWidget::ApplyInventoryWidget()
{
	if (InventoryWidget)
	{
		//InventoryWidget->SetStacks(TempItemStacks);
		//InventoryWidget->AddItem();
	}
}

void UInventoryShowcaseWidget::HandlePushRequested(const FItemStack& ItemStack)
{
	int32 FindIdx = -1; // -1 means not found. TODO: INDEX_NONE
	for (int32 ii = 0; ii < TempItemStacks.Num(); ++ii)
	{
		if (TempItemStacks[ii].ItemId == ItemStack.ItemId)
		{
			FindIdx = ii;
			break;
		}
	}

	if (FindIdx == -1)
	{
		TempItemStacks.Add(ItemStack);
	}
	else
	{
		TempItemStacks[FindIdx].Count += ItemStack.Count;
	}

	ApplyInventoryWidget();
}
