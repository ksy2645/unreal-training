// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/StreamableManager.h"
#include "UserInventory.generated.h"

/**
 * 
 */
UCLASS()
class UMGVAULT_API UUserInventory : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativePreConstruct() override;
	virtual void NativeOnInitialized() override;

private:
	TSharedPtr<FStreamableHandle> LoadHandle;
};
