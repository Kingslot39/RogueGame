// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "UmbrellaScythe.generated.h"

UCLASS()
class THEGAME_API AUmbrellaScythe : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUmbrellaScythe();
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	USkeletalMeshComponent* ScytheMesh;
	UPROPERTY(EditAnywhere)
	UBoxComponent* HitComponent;

	void ScytheActivate();
	FTimerHandle ActivateTime;
	void DestroyScythe();
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
