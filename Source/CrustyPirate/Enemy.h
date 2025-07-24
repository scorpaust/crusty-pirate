// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h"
#include "Engine/TimerHandle.h"
#include "PaperZDAnimInstance.h"
#include "Enemy.generated.h"

/**
 * 
 */
UCLASS()
class CRUSTYPIRATE_API AEnemy : public APaperZDCharacter
{
	GENERATED_BODY()

public:

	AEnemy();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USphereComponent* PlayerDetectorSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UTextRenderComponent* HPText;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UBoxComponent* AttackCollisionBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class APlayerCharacter* TargetToFollow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UPaperZDAnimSequence* AttackAnimSequence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StopDistanceToTarget = 70.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 HitPoints = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AttackDamage = 25;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackCooldownInSeconds = 3.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsAlive = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsStunned = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bCanMove = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bCanAttack = true;

	struct FTimerHandle AttackCooldownTimer;

	FZDOnAnimationOverrideEndSignature OnAttackOverrideEndDelegate;

	UFUNCTION()
	void OnPlayerDetectorSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnPlayerDetectorSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void Stun(const FDamageEvent& DamageEvent);

private:

	bool ShouldFollowTarget() const;

	void UpdateDirection(float MoveDirection);

	void UpdateHP(int32 NewHP);

	void AttackTarget();

	void OnAttackCooldownTimerTimeout();

	void OnAttackOverrideAnimEnd(bool completed); 
	
	UFUNCTION()
	void AttackBoxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	void EnableAttackCollisionBox(bool bEnabled);
};
