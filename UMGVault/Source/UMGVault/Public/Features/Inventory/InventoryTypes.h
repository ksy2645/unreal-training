// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/PrimaryAssetId.h"
#include "Data/Items/ItemDef.h"
#include "InventoryTypes.Generated.h"


// Enum 추가할 때 COUNT 바로 위에 추가해주세요
UENUM(BlueprintType)
enum class EInventorySort : uint8
{
	NameAsc,
	NameDesc,
	RarityAsc,
	RarityDesc,
	PriceAsc,
	PriceDesc,

	COUNT UMETA(Hidden) // 항상 마지막에 위치
};

USTRUCT(BlueprintType)
struct FInventoryFilter
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSet<EItemRarity> Rarity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSet<EItemType> Type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Query;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bHideLocked = false;
};

USTRUCT(BlueprintType)
struct FItemStack
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FPrimaryAssetId ItemId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 Count = 1;
};

UCLASS(BlueprintType)
class UMGVAULT_API UInventoryEntryObject : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FPrimaryAssetId ItemId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) int32 Count = 1;
};