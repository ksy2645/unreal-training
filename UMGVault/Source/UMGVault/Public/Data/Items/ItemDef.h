// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
//#include "GameplayTagContainer.h"

#include "ItemDef.generated.h"

/**
 * 
 */

 // Enum 추가할 때 COUNT 바로 위에 추가해주세요
UENUM(BlueprintType)
enum class EItemType : uint8
{
	Consumable,
	Material,
	Equipment,
	Quest,

	COUNT UMETA(Hidden) // 항상 마지막에 위치
};

// Enum 추가할 때 COUNT 바로 위에 추가해주세요
UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	Common,
	Uncommon,
	Rare,
	Epic,
	Legendary,
	Mythic,
	Ancient,

	COUNT UMETA(Hidden) // 항상 마지막에 위치
};

USTRUCT(BlueprintType)
struct UMGVAULT_API FItemRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere) FName ItemId;
	UPROPERTY(EditAnywhere) FText DisplayName;
	UPROPERTY(EditAnywhere, meta=(MultiLine)) FText Description;
	UPROPERTY(EditAnywhere) EItemType Type = EItemType::Material;
	UPROPERTY(EditAnywhere) EItemRarity Rarity = EItemRarity::Common;
	UPROPERTY(EditAnywhere) int32 MaxStack = 99;
	UPROPERTY(EditAnywhere) float Weight = 1.0f;
	UPROPERTY(EditAnywhere) int64 Price = 0;
	UPROPERTY(EditAnywhere) TSoftObjectPtr<UTexture2D> Icon;
	UPROPERTY(EditAnywhere) TSoftObjectPtr<UStaticMesh> PickupMesh;
	//UPROPERTY(EditAnywhere) FGameplayTagContainer Tags;



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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Meta")
	int64 Price = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Meta")
	int32 MaxStack = 99;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Meta")
	EItemRarity Rarity = EItemRarity::Common;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(TEXT("Item"), ItemId);
	}
};
