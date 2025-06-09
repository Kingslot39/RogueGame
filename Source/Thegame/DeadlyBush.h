// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "DeadlyBush.generated.h"

UCLASS()
class THEGAME_API ADeadlyBush : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADeadlyBush();
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	USkeletalMeshComponent* BushMesh;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UBoxComponent* DamageArea;

	FTimerHandle DestroyWaitTime;
	void WaitForDestroy();
	void DestroyBush();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
