// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/DataTable.h"
#include "ItemBakeLibrary.generated.h"
/**
 * 
 */

USTRUCT(BlueprintType)
struct FBakeItemOptions
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString TargetFolder = TEXT("/Game/Data/Items");
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bValidate = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bFailOnWarning = false;
};

UCLASS()
class CONTENTOPS_API UItemBakeLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "ContentOps|ItemBake", meta=(DevelopmentOnly))
	static bool BakeItemsFromDataTable(class UDataTable* ItemDataTable, const FBakeItemOptions& Opts, TArray<FString>& OutErrors);

};
