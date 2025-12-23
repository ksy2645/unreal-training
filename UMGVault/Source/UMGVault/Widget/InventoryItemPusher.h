// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

//#include "UItemTileEntry.h"
#include "Features/Inventory/InventoryTypes.h"
#include "Engine/AssetManager.h"

#include "InventoryItemPusher.generated.h"

/**
 * 두 개의 UI 그룹으로 구성된 인벤토리 아이템 푸셔 위젯
 * Group 1: 인벤토리에 아이템 넣기
 * 1. 아이템 목록이 보이는 콤보박스
 * 2. 아이템을 선택하면 선택된 아이템이 보이는 위젯
 * 3. 아이템 갯수를 입력하는 위젯
 * 4. 넣기 버튼
 * 
 * Group 2: 인벤토리에서 아이템 빼기 (인벤토리 위젯과 연동 필요, 상위 위젯에서 연동 제어)
 * 선택한 아이템이 있어야 활성화 됨
 * 1. 선택된 아이템이 보이는 위젯
 * 2. 아이템 개수를 입력하는 위젯 
 * 3. 빼기 버튼
 */

class UUItemTileEntry;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPushRequested, const FItemStack&, ItemStack);

UCLASS()
class UMGVAULT_API UInventoryItemPusherWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void HandlePushClicked();
	UFUNCTION()
	void HandleItemSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	void RefreshItemIdList();
	void FinalizeItemList();

	//UPROPERTY(BlueprintAssignable)
	FOnPushRequested OnPushRequested;

protected:
	// Group 1 Widgets
	UPROPERTY(meta = (BindWidget))
	class UComboBoxString* ItemComboBox;
	UPROPERTY(meta = (BindWidget))
	UUItemTileEntry* SelectedItemEntry;
	UPROPERTY(meta = (BindWidget))
	class UComboBoxString* AddItemCountComboBox;
	UPROPERTY(meta = (BindWidget))
	class UButton* AddItemButton;

	// Group 2 Widgets
	UPROPERTY(meta = (BindWidget))
	UUItemTileEntry* RemoveItemEntry;
	UPROPERTY(meta = (BindWidget))
	class USpinBox* RemoveItemCountSpinBox;
	UPROPERTY(meta = (BindWidget))
	class UButton* RemoveItemButton;

	UPROPERTY()
	TArray<FPrimaryAssetId> OrderedItemIds;
	UPROPERTY()
	TMap<FPrimaryAssetId, TWeakObjectPtr<UItemDef>> CachedItemIds;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPrimaryAssetType ItemPrimaryType = FPrimaryAssetType(TEXT("Item"));

private:
	int32 PendingItemDefsToLoad = 0;
};
