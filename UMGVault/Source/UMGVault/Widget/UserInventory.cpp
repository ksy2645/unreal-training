// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInventory.h"
#include "Systems/ItemAssetSubsystem.h"
#include "Data/Items/ItemDef.h"

void UUserInventory::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UUserInventory::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	FPrimaryAssetId ItemId("Item", "iron_ore");

	//UItemAssetSubsystem* Sub = GetGameInstance()->GetSubsystem<UItemAssetSubsystem>();
	//LoadHandle = Sub->LoadItemDefAsync(ItemId, {"UI"}, [this](UItemDef* Def){
	//	if (!IsValid(this) || !Def) return;
	//	if (false == Def->Icon.IsNull())
	//	{
	//		/*Def->Icon.
	//		Def->Icon.LoadAsync(this, [this, Def](UObject* Tex) {
	//			if (auto* T = Cast<UTexture2D>(Tex)) Image_Icon->SetBrushFromTexture(T);
	//			});*/
	//	}
	//});
}
