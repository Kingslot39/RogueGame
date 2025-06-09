// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ThegameCharacter.h"
#include "GameFramework/Actor.h"
#include "BlessingTreeSkill.generated.h"

UCLASS()
class THEGAME_API ABlessingTreeSkill : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABlessingTreeSkill();
	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* SkillTreeMesh;

	void SpawnTree();
    FTimerHandle RemainTime;
	void DestroyTree();
    
	UPROPERTY(EditAnywhere)
	AThegameCharacter* Target;

	bool isSkillActivate = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
