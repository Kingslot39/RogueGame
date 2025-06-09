// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BossSpell.h"
#include "DamageArea.h"
#include "DeadlyBush.h"
#include "DeadTree.h"
#include "HolySwordBossSkill.h"
#include "ThegameCharacter.h"

#include "Dialouge.h"
#include "DialogueStructs.h" 
#include "Components/SphereComponent.h"

#include "NiagaraFunctionLibrary.h"
#include "TherionGameUI.h"
#include "GameFramework/Character.h"
#include "BossTest.generated.h"

UCLASS()
class THEGAME_API ABossTest : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABossTest();
	UPROPERTY(EditAnywhere)
	AThegameCharacter* Target;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UNiagaraSystem* AbilityTelegraph;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
    UNiagaraSystem* RecAbilityTelegraph;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float MaxHealth;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float CurrentHealth = MaxHealth;
    bool bHasJump = false;
	bool bHasSword = false;
	UFUNCTION(BlueprintCallable)
	void JumpAttack();
	
	void LandingAttack();

	UFUNCTION(BlueprintCallable)
	void DealDamage(float Damage);
	UPROPERTY(EditAnywhere)
	FVector LandingLocation;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FVector LaunchDirection;

	UFUNCTION(BlueprintCallable)
	void CallSwordSkill();
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<AHolySwordBossSkill>HolySword;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FTimerHandle JumpingDelay;

	UPROPERTY(EditAnywhere)
	FTimerHandle Waiting;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<ADamageArea>DamageAreaClass;

	UPROPERTY(EditAnywhere)
	FVector TargetLocation;
	UPROPERTY(EditAnywhere)
	FRotator TargetRotator;

	//Second Phase Skill
	void SecondPhaseCallSword();
	void SecondPhaseSpawnTree();
	void SecondPhaseSpawnBush();
	UPROPERTY(EditAnywhere)
	float Angle;
	
	//TelegraphDelay for Skill
	//Spawn Bush
	FTimerHandle TelegraphDelay;
	void TelegraphForBush();
	//Spawn Bush Second Phase
	FTimerHandle TelegraphDelayBushPhase;
	void TelegraphBushDelay2Phase();
	// Spawn Tree
	FTimerHandle TelegraphTreeDelay;
	void TelegraphTree();
	FVector SpawnTreeLocation;
	FRotator SpawnTreeRotator;
	//Spawn Tree Second Phase
	FTimerHandle TelegraphTree2Phase;
	void TelegraphTreePhase2();

	// Firing spell
	UPROPERTY(EditAnywhere)
	TSubclassOf<ABossSpell> BulletClass;
	void Shoot(FVector Direction);

	//Spawn Tree
	UPROPERTY(EditAnywhere)
	TSubclassOf<ADeadTree>DeadTreeClass;
	
	void SpawnTree();

	// SpawnBush
	UPROPERTY(EditAnywhere)
	TSubclassOf<ADeadlyBush>DeadBushClass;
	void SpawnBush();
	//Animation
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Monrage", meta=(AllowPrivateAccess))
	UAnimMontage* SummonAni;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Monrage", meta=(AllowPrivateAccess))
	UAnimMontage* FireAni;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Monrage", meta=(AllowPrivateAccess))
	UAnimMontage* JumpAni;

    void NotWalking();
	void ContinueWalking();
	
	UPROPERTY(EditAnywhere)
	FTimerHandle NoneMoveCoolDown;

	//HealthBar UI
	UPROPERTY(EditAnywhere)
	TSubclassOf<UTherionGameUI>TherionHealthBar;
	UPROPERTY(EditAnywhere)
	UTherionGameUI* TherionGameUI;
    
    // Delay time between skill
	FTimerHandle SkillTimeDelay;
	bool bIsDelayDone;
	void DelaySkillTime();
	//Dot damage
	UFUNCTION(BlueprintCallable)
	void DotDamage(float Damage);

	// Dialogue
	// Add to the class public section
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	USphereComponent* DialogueTriggerZone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	bool bDialogueTriggered = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	bool bDialogueFinished = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	TSubclassOf<UDialouge> DialogueWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	TArray<FDialogueLine> DialogueLines;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	float DialogueDisplayTime = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	bool bAllowMultipleInteractions = false;

	UFUNCTION()
	void OnTriggerZoneEntered(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Second phase dialogue
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	TArray<FDialogueLine> SecondPhaseDialogueLines;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	bool bSecondPhaseDialogueTriggered = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	bool bSecondPhaseDialogueFinished = false;

	// Function to trigger second phase dialogue
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void TriggerSecondPhaseDialogue();

	// Function to handle death with dialogue
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void HandleDeath();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual  void Tick(float DeltaSeconds) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
