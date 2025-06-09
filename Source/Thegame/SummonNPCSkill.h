#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "SummonNPCSkill.generated.h"

UCLASS()
class THEGAME_API ASummonNPCSkill : public AActor
{
    GENERATED_BODY()
    
public:    
    ASummonNPCSkill();
    
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    // ===== NPC Character Components =====
    // Skeletal mesh for the NPC appearance
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USkeletalMeshComponent* NPCMesh;
    
    // Capsule for collision
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UCapsuleComponent* CapsuleComponent;
    
    // ===== NPC Appearance Settings =====
    UPROPERTY(EditDefaultsOnly, Category = "NPC Appearance")
    USkeletalMesh* NPCSkeletalMesh;
    
    UPROPERTY(EditDefaultsOnly, Category = "NPC Appearance")
    TSubclassOf<UAnimInstance> NPCAnimBlueprint;
    
    UPROPERTY(EditDefaultsOnly, Category = "NPC Appearance")
    UAnimMontage* IdleAnimation;
    
    UPROPERTY(EditDefaultsOnly, Category = "NPC Appearance")
    UAnimMontage* AttackAnimation;
    
    UPROPERTY(EditDefaultsOnly, Category = "NPC Appearance")
    FVector NPCScale = FVector(1.0f, 1.0f, 1.0f);
    
    // ===== Combat Settings =====
    // Attack radius
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float AttackRadius = 300.0f;
    
    // Attack damage
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float AttackDamage = 25.0f;
    
    // Attack rate (attacks per second)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float AttackRate = 2.0f;
    
    // Lifetime of the summoned NPC
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float NPCLifetime = 5.0f;
    
    // ===== Main Functions =====
    // Initialize the skill with spawn location from mouse cursor
    UFUNCTION(BlueprintCallable, Category = "Summon")
    void InitializeSkill(const FVector& MouseWorldLocation);
    
    // Find and attack nearby enemies
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void AttackNearbyEnemies();
    
    // Despawn the NPC
    UFUNCTION(BlueprintCallable, Category = "Summon")
    void DespawnNPC();
    
    // Get all enemies within radius
    UFUNCTION(BlueprintCallable, Category = "Combat")
    TArray<AActor*> GetEnemiesInRadius();
    
    // Attack a specific enemy
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void AttackEnemy(AActor* Enemy);
    
    // Play attack animation
    UFUNCTION(BlueprintCallable, Category = "Animation")
    void PlayAttackAnimation();
    
    // Visual indicator for attack radius (debug)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
    bool bShowAttackRadius = false;
    
protected:
    // Sphere component for detection
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USphereComponent* DetectionSphere;
    
    // Timer handles
    FTimerHandle LifetimeTimerHandle;
    FTimerHandle AttackTimerHandle;
    
    // Current target
    UPROPERTY()
    AActor* CurrentTarget;
    
    // List of enemies in range
    UPROPERTY()
    TArray<AActor*> EnemiesInRange;
    
    // Check if actor is a valid enemy
    bool IsValidEnemy(AActor* Actor);
    
    // Find closest enemy
    AActor* FindClosestEnemy();
    
    // Rotate NPC to face target
    void RotateToTarget(AActor* Target);
    
    // Setup the NPC appearance
    void SetupNPCAppearance();
};