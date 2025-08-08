// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerRow.h"

#include "Components/Button.h"

#include "MainMenu.h"


void UServerRow::Setup(class UMainMenu* InParent, uint32 InIndex)
{
    Parent = InParent;
    Index = InIndex;
    ServerRowButton->OnClicked.AddUniqueDynamic(this, &UServerRow::OnClickServerRowButton);
    ServerRowButton->OnHovered.AddUniqueDynamic(this, &UServerRow::OnHoverServerRowButton);
}

void UServerRow::OnClickServerRowButton()
{
    Parent->SelectIndex(Index);
}

void UServerRow::OnHoverServerRowButton()
{
    //Parent->Update
}
