// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpecialShieldForce.h"

#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "LightShieldForce.generated.h"

UCLASS()
class THEGAME_API ALightShieldForce : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALightShieldForce();
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* LightSkillMesh;

	UPROPERTY(EditAnywhere)
	float SkillDamage = 50.f;

	UPROPERTY(EditAnywhere)
	float MovementSpeed = 2000;
	UPROPERTY(EditAnywhere)
	UProjectileMovementComponent* MovementComponent;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ASpecialShieldForce> ShieldForceArea;

	bool bShieldForceActivate = false;
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector Normal, const FHitResult& Hit);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
