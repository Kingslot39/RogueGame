// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "ShieldForce.generated.h"

class AEnemy;

UCLASS()
class THEGAME_API AShieldForce : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShieldForce();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* ForceMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SlowValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SlowTime;
	
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UBoxComponent* TriggerArea;

    UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	// Map to store the original speed of each enemy to restore it later
	TMap<AEnemy*, float> EnemyOriginalSpeeds;
	
	UPROPERTY(EditAnywhere)
	FTimerHandle SlowCooldown;

	void DestroyArea();
	
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	TArray<AActor*>AffectedEnemies;

};


