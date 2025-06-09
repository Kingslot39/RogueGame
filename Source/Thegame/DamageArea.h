// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "DamageArea.generated.h"

UCLASS()
class THEGAME_API ADamageArea : public AActor
{
	GENERATED_BODY()
	
	
public:	
	// Sets default values for this actor's properties
	ADamageArea();
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UBoxComponent* DamageArea;

	void DestroyArea();

	void WaitingTime();
	FTimerHandle DestroyAreaTime;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
