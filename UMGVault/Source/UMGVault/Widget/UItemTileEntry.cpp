// Fill out your copyright notice in the Description page of Project Settings.


#include "UItemTileEntry.h"
#include "Systems/ItemAssetSubsystem.h"
#include "Systems/IconLoaderSubsystem.h"
#include "Engine/AssetManager.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Data/Items/ItemDef.h"


void UUItemTileEntry::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	UE_LOG(LogTemp, Warning, TEXT("UUItemTileEntry::NativeOnInitialized()"));
	ApplyPlaceholder();
	if (ItemId.IsValid())
		SetItemId(ItemId);

}

void UUItemTileEntry::NativePreConstruct()
{
	Super::NativePreConstruct();

#if WITH_EDITOR
	const bool bDesignTime = IsDesignTime();
#else
	const bool bDesignTime = false;
#endif

	ApplyPlaceholder();

	if (bDesignTime)
	{
		// 디자인 타임 때 Subsystem, 비동기 금지
		// 그냥 플레이스홀더로 대체 하거나, ItemId가 valid하면 그걸로 세팅
		
		UItemDef* Def = nullptr;

#if WITH_EDITOR
		if (PreviewItemDef.IsValid())
		{
			Def = PreviewItemDef.Get();
		}
		else if (!PreviewItemDef.IsNull())
		{
			Def = PreviewItemDef.LoadSynchronous();
		}
		else if (ItemId.IsValid() && UAssetManager::IsInitialized())
		{
			const FSoftObjectPath Path = UAssetManager::Get().GetPrimaryAssetPath(ItemId);
			Def = Cast<UItemDef>(Path.TryLoad());
		}
		
		if (Def)
		{
			SetItemDef(Def);
			return;
		}
#endif

		return;
	}

	if (ItemId.IsValid())
		SetItemId(ItemId);
}

void UUItemTileEntry::NativeConstruct()
{
	Super::NativeConstruct();
	//ApplyPlaceholder();
	//if (ItemId.IsValid())
	//	SetItemId(ItemId);
}

void UUItemTileEntry::NativeDestruct()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (auto* Items = GI->GetSubsystem<UItemAssetSubsystem>())
			Items->CancelByOwner(this);
		if (auto* Icons = GI->GetSubsystem<UIconLoaderSubsystem>())
			Icons->CancelByOwner(this);
	}

	if (DefHandle)
	{
		if (DefHandle->IsActive())
			DefHandle->CancelHandle();
		DefHandle.Reset();
	}

	Super::NativeDestruct();
}

void UUItemTileEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	// Super::NativeOnListItemObjectSet(ListItemObject);
	if (UItemDef* Def = Cast<UItemDef>(ListItemObject))
	{
		SetItemDef(Def);
	}
	else
	{
		ApplyPlaceholder();
	}
}


void UUItemTileEntry::SetItemId(const FPrimaryAssetId InId)
{
	PendingId = InId;
	ApplyPlaceholder();

	if (!InId.IsValid())
		return;

	if (UGameInstance* GI = GetGameInstance())
	{
		auto* Items = GI->GetSubsystem<UItemAssetSubsystem>();

		DefHandle = Items->LoadItemDefAsync(InId, { FName("UI")}, this,
			[this, InId](UItemDef* Def)
			{
				if (!IsValid(this) || !Def || InId != PendingId)
					return;

				SetItemDef(Def);
			});
	}
}

void UUItemTileEntry::SetItemDef(UItemDef* Def)
{
	if (!Def)
	{
		ApplyPlaceholder();
		return;
	}

	PendingId = Def->GetPrimaryAssetId();

	if (Text_Name)
		Text_Name->SetText(Def->DisplayName);

#if WITH_EDITOR
	const bool bDesignTime = IsDesignTime();
#else
	const bool bDesignTime = false;
#endif


#if WITH_EDITOR
	if (bDesignTime)
	{
		UTexture2D* Tex = nullptr;
		if (Def->Icon.IsValid())
		{
			Tex = Def->Icon.Get();
		}
		else if (!Def->Icon.IsNull())
		{
			Tex = Def->Icon.LoadSynchronous();
		}

		if (Image_Icon)
			Image_Icon->SetBrushFromTexture(Tex);

		return;
	}
#endif

	if (Def->Icon.IsValid())
	{
		if (UTexture2D* Tex = Def->Icon.Get())
		{
			if (Image_Icon)
				Image_Icon->SetBrushFromTexture(Tex);
		}
	}
	else if (!Def->Icon.IsNull())
	{
		if (auto* Icons = GetGameInstance()->GetSubsystem<UIconLoaderSubsystem>())
		{
			const FPrimaryAssetId CurrentId = PendingId;
			Icons->RequestIconAsync(Def->Icon, this,
				[this, CurrentId](UTexture2D* Tex)
				{
					if (!IsValid(this) || CurrentId != PendingId)
						return;

					if (Image_Icon)
						Image_Icon->SetBrushFromTexture(Tex);
				});
		}
	}
}

void UUItemTileEntry::ApplyPlaceholder()
{
	if (Text_Name)
		Text_Name->SetText(FText::FromString(TEXT("Loading...")));
	if (Image_Icon)
		Image_Icon->SetBrushFromTexture(nullptr);
}
