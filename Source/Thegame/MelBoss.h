// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HammerSlash.h"
#include "JudgmentSwordSlice.h"
#include "MelJudmentWidget.h"
#include "ThegameCharacter.h"
#include "UmbrellaBloom.h"
#include "UmbrellaScythe.h"
#include "GameFramework/Character.h"
#include "MelBoss.generated.h"

UCLASS()
class THEGAME_API AMelBoss : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMelBoss();
	AThegameCharacter* Target;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UNiagaraSystem* AbilityTelegraph;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UNiagaraSystem* RecAbilityTelegraph;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float MaxHealth;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float CurrentHealth = MaxHealth;

	UFUNCTION(BlueprintCallable)
	void DealDamage(float Damage);

	//Spawn Sword Skill
	int32 SwordSpawned = 0;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<AJudgmentSwordSlice>SwordClass;
	FTimerHandle SwordSpawnTimerHandle;
	
	void SpawnSword();
	void WaitingToRush();
	UFUNCTION(BlueprintCallable)
	void JudmentSwordRush();
	FTimerHandle DelaySwordRush;
	bool bIsCanRush = false;

	//Spawn Umbrella Skill
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<AUmbrellaBloom>UmbrellaClass;
	void UmbrellaBloomSkill();
	FVector SpawnUmbrellaLocation;
	FRotator SpawnUmbrellaRotation;
	void UmbrellaBloomTelegraph();
	FTimerHandle DelayTime;
	

	//Scythe Slash
	UPROPERTY(EditAnywhere)
	TSubclassOf<AUmbrellaScythe>ScytheClass;
	
	UFUNCTION(BlueprintCallable)
	void ScytheSlash();

	//HammerSkill
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<AHammerSlash>HammerClass;
	UFUNCTION(BlueprintCallable)
	void SlashHammer();

	//God Hammer Slash
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<AHammerSlash>GodHammerClass;
	UFUNCTION(BlueprintCallable)
	void GodSlamDown();

	// movement set up
	void UnAbleToMove();
	void AbleToMove();
	FTimerHandle StartMoving;
	int GetSpeed();


	// Delay time between skill
	FTimerHandle SkillTimeDelay;
	bool bIsDelayDone;
	void DelaySkillTime();

	// Health Bar UI
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UMelJudmentWidget>MelHealthBar;
	UPROPERTY(EditAnywhere)
	UMelJudmentWidget* MelJudmentWidget;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
