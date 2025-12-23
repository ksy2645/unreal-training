// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Engine/StreamableManager.h"

#include "UItemTileEntry.generated.h"

/**
 * 
 */
UCLASS()
class UMGVAULT_API UUItemTileEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data")
	FPrimaryAssetId ItemId;

	UFUNCTION(BlueprintCallable)
	void SetItemId(const FPrimaryAssetId InId);
	UFUNCTION(BlueprintCallable)
	void SetItemDef(class UItemDef* Def);

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	UPROPERTY(meta=(BindWidget))
	class UImage* Image_Icon;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Name;

private:
	FPrimaryAssetId PendingId;
	TSharedPtr<FStreamableHandle> DefHandle;
	void ApplyPlaceholder();

	//bool bDesignTime = false;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "Preview")
	TSoftObjectPtr<class UItemDef> PreviewItemDef;
#endif
};
