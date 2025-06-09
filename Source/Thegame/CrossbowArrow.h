// CrossbowArrow.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CrossbowArrow.generated.h"

UCLASS()
class THEGAME_API ACrossbowArrow : public AActor
{
	GENERATED_BODY()
    
public:    
	ACrossbowArrow();

protected:
	virtual void BeginPlay() override;

public:    
	// Arrow mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* ArrowMesh;
    
	// Projectile movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	class UProjectileMovementComponent* ProjectileMovement;
    
	// Damage amount
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float Damage = 30.0f;
    
	// Arrow speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")  
	float ArrowSpeed = 6000.0f;
    
	// Fixed range for the arrow
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float FixedRange = 2000.0f; // Distance in Unreal units
    
	// Owner character reference
	UPROPERTY()
	class AThegameCharacter* OwnerCharacter;
    
	// Initialize the arrow with direction
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void FireInDirection(const FVector& ShootDirection);
    
	// Set the owner character
	void SetOwnerCharacter(class AThegameCharacter* Character) { OwnerCharacter = Character; }
    
	// Handle collision
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector Normal, const FHitResult& Hit);
    
	// Lifetime
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float LifeSpan = 5.0f;

	void Tick(float DeltaTime);
	
private:
	// Store the start position and target position
	FVector StartPosition;
	FVector TargetPosition;
	bool bHasFixedTarget = false;
};