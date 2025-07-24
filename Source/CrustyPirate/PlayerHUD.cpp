// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"
#include "Components/TextBlock.h"

void UPlayerHUD::SetHP(int32 NewHP)
{
	FString Str = FString::Printf(TEXT("HP: %d"), NewHP);

	HPText->SetText(FText::FromString(Str));
}

void UPlayerHUD::SetDiamonds(int32 Amount)
{
	FString Str = FString::Printf(TEXT("Diamonds: %d"), Amount);

	DiamondsText->SetText(FText::FromString(Str));
}

void UPlayerHUD::SetLevel(int32 Index)
{
	FString Str = FString::Printf(TEXT("Level: %d"), Index);

	LevelText->SetText(FText::FromString(Str));
}