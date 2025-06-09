// WeaponHitDetector.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "WeaponHitDetector.generated.h"

class AWeaponSystem;

UCLASS()
class THEGAME_API AWeaponHitDetector : public AActor
{
    GENERATED_BODY()
    
public:    
    AWeaponHitDetector();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // Start hit detection for a weapon swing
    UFUNCTION(BlueprintCallable, Category = "Weapon|Hit Detection")
    void StartSwing();
    
    // End hit detection for a weapon swing
    UFUNCTION(BlueprintCallable, Category = "Weapon|Hit Detection")
    void EndSwing();
    
    // Set the weapon system reference
    UFUNCTION(BlueprintCallable, Category = "Weapon|Hit Detection")
    void SetWeaponSystem(AWeaponSystem* NewWeaponSystem);
    
    // Set the owning character
    UFUNCTION(BlueprintCallable, Category = "Weapon|Hit Detection")
    void SetOwningCharacter(ACharacter* NewOwner);
    
    // Toggle debug drawing
    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Hit Detection|Debug")
    bool bDrawDebug = false;

protected:
    // Perform the actual hit detection
    void PerformWeaponTrace();
    
    // Process a hit result
    void ProcessHit(const FHitResult& HitResult);
    
    // Reference to the weapon system
    UPROPERTY()
    AWeaponSystem* WeaponSystem;
    
    // Reference to the owning character
    UPROPERTY()
    ACharacter* OwningCharacter;
    
    // Is a weapon swing in progress
    bool bIsSwinging = false;
    
    // Previous transforms for the weapon meshes
    FTransform PrevWeaponTransform;
    
    // Actors already hit in this swing
    UPROPERTY()
    TSet<AActor*> AlreadyHitActors;
    
    // Trace timer handle
    FTimerHandle TraceTimerHandle;
    
    // Trace frequency
    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Hit Detection")
    float TraceFrequency = 0.02f;
    
    // Trace radius for hit detection
    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Hit Detection")
    float TraceRadius = 5.0f;
};