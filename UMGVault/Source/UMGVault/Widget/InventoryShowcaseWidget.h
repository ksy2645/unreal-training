// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Features/Inventory/InventoryTypes.h"

#include "InventoryShowcaseWidget.generated.h"

/**
 * UI 목록
 * 1. 인벤토리 아이템 푸셔 위젯
 * 2. 인벤토리 위젯
 */
UCLASS()
class UMGVAULT_API UInventoryShowcaseWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void HandlePushRequested(const FItemStack& ItemStack);

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	
	void ApplyInventoryWidget();

private:
	UPROPERTY(meta = (BindWidget))
	class UInventoryItemPusherWidget* InventoryItemPusher;
	UPROPERTY(meta = (BindWidget))
	class UInventoryWidget* InventoryWidget;

	TArray<FItemStack> TempItemStacks;
};
