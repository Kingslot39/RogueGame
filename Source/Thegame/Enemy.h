// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LevelTrigger.h"
#include "ThegameCharacter.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UCLASS()
class THEGAME_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHealth = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentHealth = MaxHealth;


	UPROPERTY(EditAnywhere)
	UBoxComponent* AxeDamageArea;
	
	
	FTimerHandle RagdollTimerHandle;
	 UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float RagdollTime = 2;

	UFUNCTION(BlueprintCallable, Category = "Health", BlueprintCallable)
	void GetDamage(float DamageAmount);
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	AThegameCharacter* Player;
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector Normal, const FHitResult& Hit);

	void Ragdoll();
	UFUNCTION()
	void StopRagdoll();
	UPROPERTY(EditAnywhere)
	int EnemyDead;
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> EnemyHealthWidgetBPClass;
	UPROPERTY(EditAnywhere, Category = "UI")
	UWidgetComponent* HealthWidgetComponent;


};
