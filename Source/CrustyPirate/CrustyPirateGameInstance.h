// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CrustyPirateGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class CRUSTYPIRATE_API UCrustyPirateGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 PlayerHP = 100;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 CollectedDiamondsAmount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsDoubleJumpUnlocked = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 CurrentLevelIndex = 1;	

	void SetPlayerHP(int32 NewHP);

	void AddDiamonds(int32 Amount);

	void ChangeLevel(int32 LevelIndex);

	UFUNCTION(BlueprintCallable)
	void RestartGame();

	void QuitGame();

private:

	void ResetPlayerStats();
};
