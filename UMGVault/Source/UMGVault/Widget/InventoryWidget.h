// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
//#include "Components/TileView.h"
//#include "Components/ComboBoxString.h"
//#include "Components/EditableTextBox.h"

#include "Features/Inventory/InventoryTypes.h"
#include "Features/Inventory/InventoryDataSource.h"


#include "InventoryWidget.generated.h"

/**
 * 
 */
UCLASS()
class UMGVAULT_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SetStacks(const TArray<FItemStack>& InStacks);

protected:
	virtual void NativeOnInitialized() override;

	UFUNCTION()
	void OnSortSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSearchTextChanged(const FText& Text);

	UPROPERTY(meta=(BindWidget))
	class UTileView* TileView_Items;
	UPROPERTY(meta = (BindWidget))
	class UComboBoxString* ComboBox_Sort;
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* EditableText_Search;

private:

	void RebuildItemList();

	UPROPERTY()
	UInventoryDataSource* DataSource;
	FInventoryFilter CurrentFilter;
	EInventorySort CurrentSort = EInventorySort::NameAsc;
};
