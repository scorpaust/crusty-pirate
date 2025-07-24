// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUD.generated.h"

/**
 * 
 */
UCLASS()
class CRUSTYPIRATE_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* HPText;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* DiamondsText;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* LevelText;

	void SetHP(int32 NewHP);

	void SetDiamonds(int32 Amount);

	void SetLevel(int32 Index);
};
