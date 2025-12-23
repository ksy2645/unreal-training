// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryTypes.h"
#include "InventoryDataSource.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class UMGVAULT_API UInventoryDataSource : public UObject
{
	GENERATED_BODY()
public:
	void SetStacks(const TArray<FItemStack>& Instacks) { Stacks = Instacks; }

	// 필터/정렬을 적용해 TileView에 넣을 리스트를 만든다.
	// OutItems: UInventoryEntryObject의 배열로 채워짐
	// OnAsyncReady: 이름/희귀도/가격 기준 정렬·검색이 필요한 경우, 비동기 작업이 끝났을 때 호출됨
	void Build(const FInventoryFilter& Filter, EInventorySort Sort, TArray<UObject*>& OutItems, TFunction<void()> OnAsyncReady);
	void AddItemStack(const FItemStack& InStacks);

private:
	TArray<FItemStack> Stacks;

	// Def 메타가 필요한 정렬/검색인지
	static bool NeedDefs(const FInventoryFilter& Filter, EInventorySort Sort);

	// Def 로드 뒤 시제 필터링/정렬 수행
	void FilterAndSort_UsingDefs(const FInventoryFilter& Filter, EInventorySort Sort, TArray<FItemStack>& InOut);

};
