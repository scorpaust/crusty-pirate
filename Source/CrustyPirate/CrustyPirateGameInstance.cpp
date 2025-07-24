// Fill out your copyright notice in the Description page of Project Settings.


#include "CrustyPirateGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UCrustyPirateGameInstance::SetPlayerHP(int32 NewHP)
{
	PlayerHP = NewHP;

	return;
}

void UCrustyPirateGameInstance::AddDiamonds(int32 Amount)
{
	CollectedDiamondsAmount += Amount;
}

void UCrustyPirateGameInstance::ChangeLevel(int32 LevelIndex)
{
	if (LevelIndex <= 0) return;

	CurrentLevelIndex = LevelIndex;

	UGameplayStatics::OpenLevel(this, FName(*FString::Printf(TEXT("Level_%d"), LevelIndex)));
}

void UCrustyPirateGameInstance::RestartGame()
{
	ResetPlayerStats();

	ChangeLevel(CurrentLevelIndex);
}

void UCrustyPirateGameInstance::QuitGame()
{
	UKismetSystemLibrary::QuitGame(this, UGameplayStatics::GetPlayerController(GetWorld(), 0), EQuitPreference::Quit, false);
}

void UCrustyPirateGameInstance::ResetPlayerStats()
{
	CurrentLevelIndex = 1;
	PlayerHP = 100;
	CollectedDiamondsAmount = 0;
	bIsDoubleJumpUnlocked = false;
}
