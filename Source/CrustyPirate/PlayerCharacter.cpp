// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "DamageType_Stun.h"
#include "Components/BoxComponent.h"
#include "Engine/DamageEvents.h"
#include "Kismet/KismetMathLibrary.h"	
#include "PlayerHUD.h"
#include "Kismet/GameplayStatics.h"
#include "CrustyPirateGameInstance.h"
#include "Enemy.h"
#include "GameFramework/CharacterMovementComponent.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));

	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	AttackCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackCollisionBox"));
	
	AttackCollisionBox->SetupAttachment(RootComponent);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}

	OnAttackOverrideEndDelegate.BindUObject(this, &APlayerCharacter::OnAttackOverrideAnimEnd);

	AttackCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::AttackBoxOverlapBegin);

	EnableAttackCollisionBox(false); // Disable attack collision box at the start

	GameInstance = Cast<UCrustyPirateGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	if (GameInstance)
	{
		// Initialize player stats from the game instance
		HitPoints = GameInstance->PlayerHP; // Default hit points, can be set from the game instance

		bIsAlive = true;
		bCanMove = true;
		bCanAttack = true;
		bIsStunned = false;
		UpdateHP(HitPoints); // Update the HP text render component with the initial hit points value

		if (GameInstance->bIsDoubleJumpUnlocked)
		{
			UnlockDoubleJump(); // Call the function to unlock double jump if it is already unlocked in the game instance
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GameInstance is null!"));
	}

	if (PlayerHUDClass)
	{
		PlayerHUDWidget = CreateWidget<UPlayerHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0), PlayerHUDClass);
		
		if (PlayerHUDWidget)
		{
			PlayerHUDWidget->AddToViewport();
			PlayerHUDWidget->SetVisibility(ESlateVisibility::Visible);
			PlayerHUDWidget->SetHP(HitPoints); // Initialize HP display
			PlayerHUDWidget->SetDiamonds(GameInstance->CollectedDiamondsAmount); // Initialize diamonds display
			PlayerHUDWidget->SetLevel(GameInstance->CurrentLevelIndex); // Initialize level display
		}
	}	
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up input action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &APlayerCharacter::JumpStarted);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &APlayerCharacter::JumpEnded);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Canceled, this, &APlayerCharacter::JumpEnded);

		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &APlayerCharacter::Attack);

		EnhancedInputComponent->BindAction(QuitAction, ETriggerEvent::Started, this, &APlayerCharacter::QuitGame);
	}
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{	
	float MoveActionValue = Value.Get<float>();

	if (bIsAlive && bCanMove)
	{
		FVector Direction = FVector(1.0f, 0.0f, 0.0f); // Default to moving right	
		
		AddMovementInput(Direction, MoveActionValue);

		UpdateDirection(MoveActionValue);
	}
}

void APlayerCharacter::JumpStarted(const FInputActionValue& Value)
{
	if (bIsAlive && bCanMove)
	{
		Jump();
	}
}

void APlayerCharacter::JumpEnded(const FInputActionValue& Value)
{
	StopJumping();
}

void APlayerCharacter::Attack(const FInputActionValue& Value)
{
	if (bIsAlive && bCanAttack)
	{
		bCanAttack = false;
		bCanMove = false;

		//EnableAttackCollisionBox(true); // Enable attack collision box

		GetAnimInstance()->PlayAnimationOverride(AttackAnimation, FName("DefaultSlot"), 1.0f, 0.0f, OnAttackOverrideEndDelegate);
	}
}

void APlayerCharacter::UpdateDirection(float MoveDirection)
{
	FRotator CurrentRotation = Controller->GetControlRotation();	

	if (MoveDirection < 0.f)
	{
		if (CurrentRotation.Yaw != 180.f)
		{
			Controller->SetControlRotation(FRotator(CurrentRotation.Pitch, 180.f, CurrentRotation.Roll)); // Face left
		}
	}

	else if (MoveDirection > 0.f)
	{
		if (CurrentRotation.Yaw != 0.0f)
		{
			Controller->SetControlRotation(FRotator(CurrentRotation.Pitch, 0.0f, CurrentRotation.Roll)); // Face right
		}
	}
}

void APlayerCharacter::OnAttackOverrideAnimEnd(bool bCompleted)
{
	if (bIsActive && bIsAlive)
	{
		bCanAttack = true;
		bCanMove = true;
	}
	//EnableAttackCollisionBox(false); // Disable attack collision box after attack animation ends
}

void APlayerCharacter::Deactivate()
{
	if (bIsActive)
	{
		bIsActive = false; // Set the active state to false

		bCanAttack = false; // Disable attack

		bCanMove = false; // Disable movement

		GetCharacterMovement()->StopMovementImmediately(); // Stop character movement immediately
	}
}

void APlayerCharacter::AttackBoxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AEnemy* Enemy = Cast<AEnemy>(OtherActor);

	if (Enemy)
	{
		FDamageEvent DamageEvent(UDamageType_Stun::StaticClass());

		UDamageType_Stun* StunType = Cast<UDamageType_Stun>(DamageEvent.DamageTypeClass->GetDefaultObject());

		StunType->StunDuration = UKismetMathLibrary::RandomFloatInRange(1.f, 3.f); // Set the stun duration for the enemy

		Enemy->TakeDamage((float)AttackDamage, DamageEvent, Controller, this); // Call the TakeDamage function on the enemy
	}
}

void APlayerCharacter::EnableAttackCollisionBox(bool bEnabled)
{
	if (bEnabled)
	{
		AttackCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		AttackCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	}
	else
	{
		AttackCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		AttackCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	}
}

float APlayerCharacter::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!bIsAlive || !bIsActive)
	{
		return 0.f;
	}
	else
	{
		HitPoints -= DamageAmount; // Reduce hit points by the damage amount

		GetAnimInstance()->JumpToNode(FName("JumpTakeHit"), FName("CaptainStateMachine")); // Jump to the hit animation node in the animation state machine

		if (HitPoints <= 0)
		{
			bIsAlive = false; // Mark the enemy as dead if hit points drop to zero or below
			HitPoints = 0; // Ensure hit points do not go below zero
			// HPText->SetHiddenInGame(true); // Hide the HP text render component when the enemy is dead
			// Handle enemy death logic here, like playing a death animation or sound
			EnableAttackCollisionBox(false); // Disable the attack collision box when dead
			GetAnimInstance()->JumpToNode(FName("JumpDie"), FName("CaptainStateMachine")); // Jump to the die animation node in the animation state machine
		
			GetWorldTimerManager().SetTimer(RestartTimer, this, &APlayerCharacter::OnRestartTimerTimeout, 3.f, false); // Set a timer to restart the game after 3 seconds
		}

		UpdateHP(HitPoints); // Update the HP text render component with the new hit points value

		Stun(DamageEvent);

		return DamageAmount;  // Return the damage amount taken
	}

	return 0.f;  // Return the damage amount taken
}
	
void APlayerCharacter::OnRestartTimerTimeout()
{
	GameInstance->RestartGame(); // Call the RestartGame function in the game instance to restart the game
}

void APlayerCharacter::UpdateHP(int32 NewHP)
{
	HitPoints = NewHP;

	if (PlayerHUDWidget)
	{
		GameInstance->SetPlayerHP(HitPoints); // Update the player HP in the game instance	
		PlayerHUDWidget->SetHP(HitPoints); // Update the HP display in the HUD widget
	}
}

void APlayerCharacter::Stun(const FDamageEvent& DamageEvent)
{	
	if (DamageEvent.DamageTypeClass)
	{
		const UDamageType_Stun* StunType = Cast<UDamageType_Stun>(DamageEvent.DamageTypeClass->GetDefaultObject());
		if (StunType && StunType->StunDuration > 0.f)
		{
			bCanMove = false;
			bIsStunned = true; // Set the stunned state to true
			GetAnimInstance()->StopAllAnimationOverrides(); // Stop all animation overrides to ensure the enemy plays the stun animation// Reset the stunned state after the stun duration
			GetWorldTimerManager().SetTimerForNextTick([this, Duration = StunType->StunDuration]()
				{
					FTimerHandle UnusedHandle;
					GetWorldTimerManager().SetTimer(UnusedHandle, [this]() {
						bCanMove = true;
						bIsStunned = false;
						}, Duration, false);
				});

		}
	}
}

void APlayerCharacter::CollectItem(ECollectableType ItemType)
{
	UGameplayStatics::PlaySound2D(this, ItemPickupSound); // Play the item collection sound

	switch (ItemType)
	{
	case ECollectableType::HealthPotion:
	{
		int32 HealAmount = 25; // Amount to heal

		UpdateHP(HitPoints + HealAmount); // Increase hit points by 25 when collecting a health potion
	} break;

	case ECollectableType::Diamond:
	{
		GameInstance->AddDiamonds(1);

		PlayerHUDWidget->SetDiamonds(GameInstance->CollectedDiamondsAmount); // Update the diamonds display in the HUD widget
	} break;

	case ECollectableType::DoubleJumpUpgrade:
	{
		if (!GameInstance->bIsDoubleJumpUnlocked)
		{
			GameInstance->bIsDoubleJumpUnlocked = true; // Unlock double jump in the game instance

			UnlockDoubleJump(); // Call the function to unlock double jump
		}
	} break;

	default:
	{
	} break;
	}
}

void APlayerCharacter::UnlockDoubleJump()
{
	JumpMaxCount = 2; // Set the maximum jump count to 2 for double jump
}

void APlayerCharacter::QuitGame(const FInputActionValue& Value)
{
	if (bIsAlive && bIsActive)
	{
		if (GameInstance)
		{
			GameInstance->QuitGame(); // Call the QuitGame function in the game instance to quit the game
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("GameInstance is null! Cannot quit game."));
		}
	}
}
