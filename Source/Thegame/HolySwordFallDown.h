// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "HolySwordFallDown.generated.h"

UCLASS()
class THEGAME_API AHolySwordFallDown : public AActor
{
	GENERATED_BODY()
	

	
public:
	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* SwordMesh;
	// Sets default values for this actor's properties
	AHolySwordFallDown();
	//Destroy function
	void DestroySword();
	void PlaySwordFallDown();
	void FadingOutSword();
	FTimerHandle DelayTime;
	FTimerHandle DestroyTime;

	UPROPERTY(EditAnywhere)
	AActor* Player;
	UPROPERTY(EditAnywhere)
	float Progress;
	UPROPERTY(EditAnywhere)
	UMaterialInstanceDynamic* DynamicMaterial;
	
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
