// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemBakeLibrary.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/DataValidation.h"
#include "UObject/SavePackage.h"

#include "Data/Items/ItemDef.h"

static UItemDef* LoadOrCreateItemDef(const FString& PkgPath, const FName& AssetName)
{
	const FString ObjectPath = PkgPath + TEXT(".") + AssetName.ToString();
	// 이미 있으면 로드
	if (UObject* Existing = StaticLoadObject(UItemDef::StaticClass(), nullptr, *ObjectPath))
	{
		return Cast<UItemDef>(Existing);
	}

	// 없으면 새로 생성 후 등록
	UPackage* Package = CreatePackage(*PkgPath);
	UItemDef* NewItemDef = NewObject<UItemDef>(Package, UItemDef::StaticClass(), AssetName, RF_Public | RF_Standalone);
	FAssetRegistryModule::AssetCreated(NewItemDef);

	Package->MarkPackageDirty(); // 변경된 페키지 상태 표시 -> 에디터 하단에 저장 필요 표시됨
	return NewItemDef;
}

// TODO: PreFix(DA_) 문구 하드코딩 제거
bool UItemBakeLibrary::BakeItemsFromDataTable(UDataTable* ItemDataTable, const FBakeItemOptions& Opts, TArray<FString>& OutErrors)
{
#if WITH_EDITOR
	if (!ItemDataTable)
	{
		OutErrors.Add(TEXT("ItemDataTable is Null."));
		return false;
	}

	UE_LOG(LogTemp, Display, TEXT("Starting ItemDataTable Bake"));
	const TArray<FName> RowNames = ItemDataTable->GetRowNames();
	int32 Updated = 0;

	for (const FName RowName : RowNames)
	{
		static const FString Ctx = TEXT("BakeItemsFromDataTable");
		const FItemRow* ItemRow = ItemDataTable->FindRow<FItemRow>(RowName, Ctx);
		if (!ItemRow)
		{
			OutErrors.Add(FString::Printf(TEXT("Failed to find row: %s"), *RowName.ToString()));
			continue;
		}

		if (ItemRow->ItemId.IsNone())
		{
			OutErrors.Add(FString::Printf(TEXT("ItemId is None in row: %s"), *RowName.ToString()));
			continue;
		}

		const FString PackagePath = Opts.TargetFolder / TEXT("DA_") + ItemRow->ItemId.ToString();
		const FName AssetName(*FString::Printf(TEXT("DA_%s"), *ItemRow->ItemId.ToString()));
		UItemDef* ItemDef = LoadOrCreateItemDef(PackagePath, AssetName);
		if (!ItemDef)
		{
			OutErrors.Add(FString::Printf(TEXT("Create Load failed: %s"), *PackagePath));
			continue;
		}

		ItemDef->ItemId = ItemRow->ItemId;
		ItemDef->DisplayName = ItemRow->DisplayName;
		ItemDef->Description = ItemRow->Description;
		ItemDef->Icon = ItemRow->Icon;
		ItemDef->Price = ItemRow->Price;
		ItemDef->Rarity = ItemRow->Rarity;
		ItemDef->MaxStack = ItemRow->MaxStack;

		if (Opts.bValidate)
		{
			FDataValidationContext ValidationCtx;
			const EDataValidationResult R = ItemDef->IsDataValid(ValidationCtx);

			// EMessageSeverity::Warning;
			// EMessageSeverity::Error;
			for (const FDataValidationContext::FIssue& I : ValidationCtx.GetIssues())
			{
				if (I.Severity == EMessageSeverity::Error)
				{
					OutErrors.Add(I.Message.ToString());
				}
				else if (Opts.bFailOnWarning && (I.Severity == EMessageSeverity::Warning || I.Severity == EMessageSeverity::PerformanceWarning))
				{
					OutErrors.Add(I.Message.ToString());
				}
			}

			if (R == EDataValidationResult::Invalid || ValidationCtx.GetNumWarnings() > 0 || ValidationCtx.GetNumErrors() > 0)
			{
				continue;
			}
		}

		UPackage* Package = ItemDef->GetPackage(); // GetOutermost()
		Package->MarkPackageDirty();
		FSavePackageArgs Args;
		Args.TopLevelFlags = RF_Public | RF_Standalone;
		UPackage::SavePackage(Package, ItemDef, *FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension()), Args);
		++Updated;
	}

	UE_LOG(LogTemp, Display, TEXT("Bake done: %d updated, %d issues"), Updated, OutErrors.Num());
	return OutErrors.Num() == 0;
#else
	OutErrors.ADD(TEXT("This function can only be run in the editor."));
	return false;
#endif
}
