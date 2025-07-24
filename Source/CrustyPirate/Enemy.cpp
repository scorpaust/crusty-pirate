// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "Engine/DamageEvents.h"
#include "Components/TextRenderComponent.h"
#include "Components/BoxComponent.h"
#include "PlayerCharacter.h"
#include "PaperZDAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "DamageType_Stun.h"


AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create the player detector sphere component
	PlayerDetectorSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PlayerDetectorSphere"));
	PlayerDetectorSphere->InitSphereRadius(200.0f);
	PlayerDetectorSphere->SetupAttachment(RootComponent);

	// Create the attack collision box component
	AttackCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackCollisionBox"));
	AttackCollisionBox->SetupAttachment(RootComponent);

	// Create the text render component for displaying HP
	HPText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRender"));
	HPText->SetupAttachment(RootComponent);

	// Bind overlap events
	PlayerDetectorSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnPlayerDetectorSphereBeginOverlap);
	PlayerDetectorSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnPlayerDetectorSphereEndOverlap);	
	TargetToFollow = nullptr; // Initialize the target to follow as null
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	UpdateHP(HitPoints); // Initialize the HP text render component with the current hit points

	OnAttackOverrideEndDelegate.BindUObject(this, &AEnemy::OnAttackOverrideAnimEnd); // Bind the attack override end delegate to the OnAttackOverrideEnd function

	AttackCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AttackBoxOverlapBegin); // Bind the attack collision box overlap begin event

	EnableAttackCollisionBox(false); // Disable the attack collision box initially
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsAlive && TargetToFollow && !bIsStunned)
	{
		float MoveDirection = (TargetToFollow->GetActorLocation().X - GetActorLocation().X) > 0.0f ? 1.0f : -1.0f; // Determine direction to move towards the player	
	
		UpdateDirection(MoveDirection); // Update the direction based on the player's position

		if (ShouldFollowTarget())
		{
			if (bCanMove)
			{
				FVector WorldDirection = FVector(1.0f, 0.0f, 0.0f); // Assuming movement is along the X-axis
				AddMovementInput(WorldDirection, MoveDirection);
			}
		}
		else
		{
			// Attack the player if close enough
			if (TargetToFollow->bIsAlive && bCanAttack)
			{
				AttackTarget(); // Call the attack function to attack the player
			}
		}
		
	}
}

void AEnemy::OnPlayerDetectorSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);	

	if (PlayerCharacter && PlayerCharacter->IsPlayerControlled())
	{
		// Handle player detection logic here
		
		TargetToFollow = PlayerCharacter;	 // Set the player character as the target to follow	

		// You can add more logic here, like triggering an attack or alerting the enemy
	}
}

void AEnemy::OnPlayerDetectorSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);

	if (PlayerCharacter && PlayerCharacter->IsPlayerControlled())
	{
		// Handle end player detection logic here

		TargetToFollow = nullptr; // Clear the target if the overlapping actor is not a player character

		// You can add more logic here, like triggering an attack or alerting the enemy
	}
}

float AEnemy::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!bIsAlive)
	{
		return 0.f;
	}
	else
	{
		HitPoints -= DamageAmount; // Reduce hit points by the damage amount

		GetAnimInstance()->JumpToNode(FName("JumpTakeHit"), FName("CrabbyStateMachine")); // Jump to the hit animation node in the animation state machine

		if (HitPoints <= 0)
		{
			bIsAlive = false; // Mark the enemy as dead if hit points drop to zero or below
			HitPoints = 0; // Ensure hit points do not go below zero
			HPText->SetHiddenInGame(true); // Hide the HP text render component when the enemy is dead
			// Handle enemy death logic here, like playing a death animation or sound
			EnableAttackCollisionBox(false); // Disable the attack collision box when dead
		}

		UpdateHP(HitPoints); // Update the HP text render component with the new hit points value

		Stun(DamageEvent);
	}	

	// Morte, se HP <= 0
	if (HitPoints <= 0)
	{
		bIsAlive = false; // Set the alive state to false
		bCanAttack = false;// Disable attack ability when dead
		bCanMove = false; // Disable movement when dead			

		GetAnimInstance()->JumpToNode(FName("JumpDie"), FName("CrabbyStateMachine")); // Jump to the death animation node in the animation state machine
		// Death logic...
	}

	return DamageAmount;
}

void AEnemy::Stun(const FDamageEvent& DamageEvent)
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

bool AEnemy::ShouldFollowTarget() const
{
	bool Result = false;

	if (TargetToFollow)
	{
		float DistanceToTarget = abs(TargetToFollow->GetActorLocation().X - GetActorLocation().X);

		Result = DistanceToTarget > StopDistanceToTarget;
	}

	return Result;
}

void AEnemy::UpdateDirection(float MoveDirection)
{
	FRotator CurrentRotation = GetActorRotation();

	if (MoveDirection < 0.f)
	{
		if (CurrentRotation.Yaw != 180.f)
		{
			SetActorRotation(FRotator(CurrentRotation.Pitch, 180.f, CurrentRotation.Roll)); // Face left
		}
	}

	else if (MoveDirection > 0.f)
	{
		if (CurrentRotation.Yaw != 0.0f)
		{
			SetActorRotation(FRotator(CurrentRotation.Pitch, 0.0f, CurrentRotation.Roll)); // Face right
		}
	}
}

void AEnemy::UpdateHP(int32 NewHP)
{
	HitPoints = NewHP;

	FString Str = FString::Printf(TEXT("HP: %d"), HitPoints);

	HPText->SetText(FText::FromString(Str)); // Update the text render component with the new HP value
}

void AEnemy::AttackTarget()
{
	if (bIsAlive && bCanAttack && !bIsStunned)
	{
		bCanAttack = false; // Disable further attacks until the cooldown is over
		bCanMove = false; // Disable movement while attacking
		GetAnimInstance()->PlayAnimationOverride(AttackAnimSequence, FName("DefaultSlot")); // Play the attack animation override
		GetWorldTimerManager().SetTimer(AttackCooldownTimer, this, &AEnemy::OnAttackCooldownTimerTimeout, 1.0f, false, AttackCooldownInSeconds);

	}
}

void AEnemy::OnAttackCooldownTimerTimeout()
{
	if (bIsAlive)
	{
		bCanAttack = true; // Re-enable attack ability after the cooldown
	}
}

void AEnemy::OnAttackOverrideAnimEnd(bool completed)
{
	if (bIsAlive)
	{
		bCanMove = true; // Re-enable movement after the attack animation ends
	}
}

void AEnemy::AttackBoxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor); // Check if the overlapping actor is a player character

	if (Player) // If it's a player character and the enemy can attack
	{
		FDamageEvent DamageEvent(UDamageType_Stun::StaticClass());

		UDamageType_Stun* StunType = Cast<UDamageType_Stun>(DamageEvent.DamageTypeClass->GetDefaultObject());

		StunType->StunDuration = UKismetMathLibrary::RandomFloatInRange(1.f, 3.f); // Set the stun duration for the enemy

		Player->TakeDamage((float)AttackDamage, DamageEvent, GetController(), this); // Apply damage to the player character
	}
}

void AEnemy::EnableAttackCollisionBox(bool bEnabled)
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