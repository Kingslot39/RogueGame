// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HitStop.generated.h"

UCLASS()
class THEGAME_API AHitStop : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHitStop();
	/** How slow the world gets (0.01 = almost frozen). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HitStop")
	float SlowMoFactor = 0.01f;

	/** How long to stay in slow-mo (seconds). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HitStop")
	float Duration = 0.1f;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// to remember the original time dilation
	float OriginalDilation;

	FTimerHandle TimerHandle_EndHitStop;
	void EndHitStop();
};
