// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BossSpell.h"
#include "GameFramework/Actor.h"
#include "SpellTreeSkill.generated.h"

UCLASS()
class THEGAME_API ASpellTreeSkill : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpellTreeSkill();
	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* SpellTreeMesh;

	UFUNCTION()
	void Shooting();
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<ABossSpell>SpellClass;
	UPROPERTY(EditAnywhere)
	float ShootingDelay;
	bool bCanShoot;
	float FieldOfView;
	float AttackRange;
	float ReloadTime;
    UPROPERTY(EditAnywhere)
	AActor* Target;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
