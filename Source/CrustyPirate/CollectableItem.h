// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CollectableItem.generated.h"

UENUM(BlueprintType)
enum class ECollectableType : uint8
{
	Diamond UMETA(DisplayName = "Diamond"),
	HealthPotion UMETA(DisplayName = "Health Potion"),
	DoubleJumpUpgrade UMETA(DisplayName = "Double Jump Upgrade"),
};

UCLASS()
class CRUSTYPIRATE_API ACollectableItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACollectableItem();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCapsuleComponent* CapsuleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UPaperFlipbookComponent* FlipbookItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECollectableType CollectableType;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
