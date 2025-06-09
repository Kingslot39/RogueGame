// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "RotatingShield.generated.h"

UCLASS()
class THEGAME_API ARotatingShield : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARotatingShield();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneRoot;
    
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ShieldMesh;
    
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* DamageCollision;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UNiagaraComponent* ShieldEffect;

	// Shield Properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield Properties")
	float RotationSpeed = 120.0f; // Degrees per second
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield Properties")
	float OrbitRadius = 200.0f; // Distance from player
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield Properties")
	float ShieldDamage = 25.0f;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield Properties")
	float StunDuration = 1.5f;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield Properties")
	float DamageInterval = 0.5f; // Time between damage ticks for same enemy

	// Owner reference
	UPROPERTY()
	class AThegameCharacter* OwnerCharacter;
    
	// Current rotation angle
	float CurrentAngle = 0.0f;
    
	// Track damaged enemies to implement damage cooldown
	UPROPERTY()
	TMap<AActor*, float> DamagedEnemiesTimestamp;
    
	// Functions
	UFUNCTION(BlueprintCallable, Category = "Shield")
	void InitializeShield(class AThegameCharacter* OwningCharacter);
    
	UFUNCTION()
	void OnShieldBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, 
		const FHitResult& SweepResult);
    
	void UpdateShieldPosition(float DeltaTime);
	bool CanDamageEnemy(AActor* Enemy);
	void ApplyStunToEnemy(class AEnemy* Enemy);
};
