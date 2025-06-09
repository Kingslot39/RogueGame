// CrossbowArrow.cpp
#include "CrossbowArrow.h"
#include "ThegameCharacter.h"
#include "Enemy.h"
#include "BossTest.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"

ACrossbowArrow::ACrossbowArrow()
{
    PrimaryActorTick.bCanEverTick = true; // Enable tick to check distance
    
    // Create root component
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
    
    // Create arrow mesh
    ArrowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArrowMesh"));
    ArrowMesh->SetupAttachment(RootComponent);
    ArrowMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    ArrowMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
    ArrowMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
    ArrowMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
    
    // Create projectile movement component
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->UpdatedComponent = ArrowMesh;
    ProjectileMovement->InitialSpeed = 3000.0f; // Default speed
    ProjectileMovement->MaxSpeed = 10000.0f; // Allow higher speeds
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;
    ProjectileMovement->ProjectileGravityScale = 0.0f; // Disable gravity
    ProjectileMovement->bSweepCollision = true;
    
    // Set lifespan
    InitialLifeSpan = LifeSpan;
}

void ACrossbowArrow::BeginPlay()
{
    Super::BeginPlay();
    
    // Bind hit event
    ArrowMesh->OnComponentHit.AddDynamic(this, &ACrossbowArrow::OnHit);
    ArrowMesh->SetNotifyRigidBodyCollision(true);
    
    // Store start position
    StartPosition = GetActorLocation();
    
    // Ignore owner collision
    if (OwnerCharacter)
    {
        ArrowMesh->IgnoreActorWhenMoving(OwnerCharacter, true);
        
        // Also ignore the owner's capsule component specifically
        UCapsuleComponent* OwnerCapsule = OwnerCharacter->GetCapsuleComponent();
        if (OwnerCapsule)
        {
            ArrowMesh->IgnoreComponentWhenMoving(OwnerCapsule, true);
        }
    }
}

void ACrossbowArrow::FireInDirection(const FVector& ShootDirection)
{
    // Normalize the direction to ensure consistent behavior
    FVector NormalizedDirection = ShootDirection.GetSafeNormal();
    
    // Double-check we have a valid direction
    if (NormalizedDirection.IsNearlyZero())
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid shoot direction!"));
        Destroy();
        return;
    }
    
    // Calculate target position based on fixed range
    StartPosition = GetActorLocation();
    TargetPosition = StartPosition + (NormalizedDirection * FixedRange);
    bHasFixedTarget = true;
    
    // Update projectile movement speed from our ArrowSpeed variable
    ProjectileMovement->InitialSpeed = ArrowSpeed;
    ProjectileMovement->MaxSpeed = ArrowSpeed;
    
    // Set the projectile's velocity
    ProjectileMovement->Velocity = NormalizedDirection * ArrowSpeed;
    
    UE_LOG(LogTemp, Warning, TEXT("Arrow fired - Direction: %s, Speed: %f, Fixed Range: %f"), 
        *NormalizedDirection.ToString(), ArrowSpeed, FixedRange);
}

void ACrossbowArrow::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
    // Don't hit the owner
    if (OtherActor == OwnerCharacter || OtherActor == this)
    {
        return;
    }

    // Check if we hit an enemy
    AEnemy* HitEnemy = Cast<AEnemy>(OtherActor);
    ABossTest* HitBoss = Cast<ABossTest>(OtherActor);
    
    if (HitEnemy)
    {
        // Calculate damage based on owner's damage multiplier
        float FinalDamage = Damage;
        if (OwnerCharacter)
        {
            FinalDamage = Damage * (OwnerCharacter->GetCurrentDamage() / OwnerCharacter->BaseDamageAmount);
        }
        
        // Use the enemy's GetDamage function like in LightArrow
        HitEnemy->GetDamage(FinalDamage);
        
        UE_LOG(LogTemp, Warning, TEXT("Crossbow arrow hit enemy for %f damage"), FinalDamage);
    }
    else if (HitBoss)
    {
        // Apply damage to boss using the standard damage system
        float FinalDamage = Damage;
        if (OwnerCharacter)
        {
            FinalDamage = Damage * (OwnerCharacter->GetCurrentDamage() / OwnerCharacter->BaseDamageAmount);
        }
        
        UGameplayStatics::ApplyDamage(HitBoss, FinalDamage, nullptr, this, UDamageType::StaticClass());
        
        UE_LOG(LogTemp, Warning, TEXT("Crossbow arrow hit boss for %f damage"), FinalDamage);
    }
    
    // Destroy the arrow after hit
    Destroy();
}

void ACrossbowArrow::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    // Check if we've reached the fixed range
    if (bHasFixedTarget)
    {
        float DistanceTraveled = FVector::Dist(StartPosition, GetActorLocation());
        
        // If we've traveled the fixed distance, destroy the arrow
        if (DistanceTraveled >= FixedRange)
        {
            Destroy();
        }
    }
}