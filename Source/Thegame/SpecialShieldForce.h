// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "SpecialShieldForce.generated.h"

UCLASS()
class THEGAME_API ASpecialShieldForce : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpecialShieldForce();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* TrapArea;
	UFUNCTION()
	

	void AreaDestroy();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
