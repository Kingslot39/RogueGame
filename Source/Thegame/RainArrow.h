#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "RainArrow.generated.h"

UCLASS()
class THEGAME_API ARainArrow : public AActor
{
	GENERATED_BODY()
    
public:    
	ARainArrow();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
			   FVector NormalImpulse, const FHitResult& Hit);

public:    
	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ArrowMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProjectileMovementComponent* ProjectileMovement;

	// Arrow properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arrow")
	float Damage = 25.0f;
};