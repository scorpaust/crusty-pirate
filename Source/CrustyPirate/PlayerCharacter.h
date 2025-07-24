// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/InputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/Controller.h"
#include "PaperZDAnimInstance.h"
#include "CollectableItem.h"
#include "Sound/SoundBase.h"
#include "PlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class CRUSTYPIRATE_API APlayerCharacter : public APaperZDCharacter
{
	GENERATED_BODY()
	
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UTextRenderComponent* HPText;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UBoxComponent* AttackCollisionBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* QuitAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UPaperZDAnimSequence* AttackAnimation;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UPlayerHUD> PlayerHUDClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UPlayerHUD* PlayerHUDWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsAlive = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsActive = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsStunned = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bCanMove = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bCanAttack = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 HitPoints = 100;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCrustyPirateGameInstance* GameInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* ItemPickupSound;

	FZDOnAnimationOverrideEndSignature OnAttackOverrideEndDelegate;

	APlayerCharacter();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void Move(const FInputActionValue& Value);

	void JumpStarted(const FInputActionValue& Value);
	
	void JumpEnded(const FInputActionValue& Value);
	
	void Attack(const FInputActionValue& Value);

	void UpdateDirection(float MoveDirection);

	void OnAttackOverrideAnimEnd(bool bCompleted);	
	
	UFUNCTION(BlueprintCallable)
	void Deactivate();

	UFUNCTION()
	void AttackBoxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	void EnableAttackCollisionBox(bool bEnabled);	

	UFUNCTION()
	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void Stun(const FDamageEvent& DamageEvent);

	void CollectItem(ECollectableType ItemType);

	void UnlockDoubleJump();

	void QuitGame(const FInputActionValue& Value);

private:

	int32 AttackDamage = 25;

	float AttackStunDuration = 0.3f;

	FTimerHandle RestartTimer;

	void OnRestartTimerTimeout();

	void UpdateHP(int32 NewHP);
};
