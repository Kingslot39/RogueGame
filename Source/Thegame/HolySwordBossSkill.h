// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HolySwordBossSkill.generated.h"

UCLASS()
class THEGAME_API AHolySwordBossSkill : public AActor
{
	GENERATED_BODY()
	
	
public:	
	// Sets default values for this actor's properties
	AHolySwordBossSkill();
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	USkeletalMeshComponent* SwordMesh;


	UPROPERTY(EditAnywhere)
	FTimerHandle SlashTimeHandle;
	void SlashDown();

	//Change the progress to make the sword dissapear
	FTimerHandle WaitingToFadeTime;
	void FadingOut();
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float Progress = 0;

	void DestroySword();
	UPROPERTY(EditAnywhere)
	UMaterialInstanceDynamic* DynamicMaterial;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UAnimMontage* SlashdownAnimation;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
