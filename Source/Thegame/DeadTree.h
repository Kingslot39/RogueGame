// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DeadTree.generated.h"

UCLASS()
class THEGAME_API ADeadTree : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADeadTree();
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* TreeMesh;

	FTimerHandle TreeTime;
	void SetUpTree();
	void DestroyTree();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
