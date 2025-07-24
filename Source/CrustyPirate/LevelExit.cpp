// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelExit.h"
#include "Components/BoxComponent.h"
#include "PaperFlipbookComponent.h"
#include "Sound/SoundBase.h"
#include "Engine/TimerHandle.h"
#include "PlayerCharacter.h"
#include "CrustyPirateGameInstance.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ALevelExit::ALevelExit()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));

	SetRootComponent(TriggerBox);

	DoorFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("DoorFlipbook"));

	DoorFlipbook->SetupAttachment(RootComponent);

	DoorFlipbook->SetPlayRate(0.0f);

	DoorFlipbook->SetLooping(false);
}

// Called when the game starts or when spawned
void ALevelExit::BeginPlay()
{
	Super::BeginPlay();
	
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ALevelExit::OverlapBegin);

	DoorFlipbook->SetPlaybackPosition(0.0f, false);
}

// Called every frame
void ALevelExit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void ALevelExit::OverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);

	if (Player && Player->bIsAlive && bIsActive)
	{
		bIsActive = false;

		Player->Deactivate();

		DoorFlipbook->SetPlayRate(1.0f);

		DoorFlipbook->PlayFromStart();

		UGameplayStatics::PlaySound2D(this, PlayerEnterSound);

		GetWorldTimerManager().SetTimer(WaitTimer, this, &ALevelExit::OnWaitTimerTimeout, 1.0f, false, WaitTimeInSeconds);
	}
}

void ALevelExit::OnWaitTimerTimeout()
{
	UCrustyPirateGameInstance* MyGameInstance = Cast<UCrustyPirateGameInstance>(GetGameInstance());

	if (MyGameInstance)	
	{
		MyGameInstance->ChangeLevel(LevelIndex);
	}
}

