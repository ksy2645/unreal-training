// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerRow.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLEPLATFORMS_API UServerRow : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ServerNameText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HostUser;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ConnectionFraction;

	UPROPERTY(BlueprintReadOnly)
	bool Selected = false;

	void Setup(class UMainMenu* InParent, uint32 InIndex);

private:
	UPROPERTY(meta = (BindWidget))
	class UButton* ServerRowButton;

	UPROPERTY()
	class UMainMenu* Parent;

	UFUNCTION()
	void OnClickServerRowButton();

	UFUNCTION()
	void OnHoverServerRowButton();

	uint32 Index;
};
