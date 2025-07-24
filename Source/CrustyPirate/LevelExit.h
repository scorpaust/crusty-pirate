// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelExit.generated.h"

UCLASS()
class CRUSTYPIRATE_API ALevelExit : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelExit();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UBoxComponent* TriggerBox; // Box component to detect player overlap

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UPaperFlipbookComponent* DoorFlipbook; // Flipbook component for visual representation

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USoundBase* PlayerEnterSound; // Sound to play when player enters the exit

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 LevelIndex = 1; // Index of the level to load when exiting

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WaitTimeInSeconds = 2.0f; // Time to wait before changing the level

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsActive = true; // Flag to check if the exit is active

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:

	struct FTimerHandle WaitTimer; // Timer handle for managing wait time before level change

	void OnWaitTimerTimeout(); // Function called when wait timer times out
};
