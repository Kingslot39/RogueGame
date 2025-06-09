// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UmbrellaBloom.generated.h"

UCLASS()
class THEGAME_API AUmbrellaBloom : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUmbrellaBloom();
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	USkeletalMeshComponent* UmbrellaMesh;

    //Activate Skill
	void SkillRun();
	void DestroyUmbrella();
	FTimerHandle DestroyTime;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
