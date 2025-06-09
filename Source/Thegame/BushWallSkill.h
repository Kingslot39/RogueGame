// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BushWallSkill.generated.h"

UCLASS()
class THEGAME_API ABushWallSkill : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABushWallSkill();
	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* BushWallMesh;

	void SpawnBush();
	void DestroyBush();
	FTimerHandle SpawningTime;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
