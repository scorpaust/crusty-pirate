// Fill out your copyright notice in the Description page of Project Settings.


#include "CollectableItem.h"
#include "Components/CapsuleComponent.h"
#include "PaperFlipbookComponent.h"
#include "PlayerCharacter.h" 

// Sets default values
ACollectableItem::ACollectableItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Initialize the capsule component	
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));

	SetRootComponent(CapsuleComponent);


	// Initialize the flipbook component
	FlipbookItem = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("FlipbookItem"));

	FlipbookItem->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ACollectableItem::BeginPlay()
{
	Super::BeginPlay();
	
	CapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &ACollectableItem::OverlapBegin);
}

// Called every frame
void ACollectableItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACollectableItem::OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);

	if (PlayerCharacter && PlayerCharacter->bIsAlive)
	{
		PlayerCharacter->CollectItem(CollectableType);

		Destroy(); // Destroy the item after collection
	}
}

