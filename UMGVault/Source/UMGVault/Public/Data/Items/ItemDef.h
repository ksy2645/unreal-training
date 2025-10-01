// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "ItemDef.generated.h"

/**
 * 
 */


UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	Common,
	Uncommon,
	Rare,
	Epic,
	Legendary,
	Mythic,
	Ancient
};

UCLASS(BlueprintType)
class UMGVAULT_API UItemDef : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ID")
	FName ItemId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Display")
	FText DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Display", meta=(MultiLine))
	FText Description;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Display", meta=(AssetBundles="UI"))
	TSoftObjectPtr<UTexture2D> Icon;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Meta")
	EItemRarity Rarity = EItemRarity::Common;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(TEXT("Item"), ItemId);
	}
};
