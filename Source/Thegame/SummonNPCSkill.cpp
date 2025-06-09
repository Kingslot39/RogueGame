#include "SummonNPCSkill.h"
#include "Enemy.h"
#include "BossTest.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Animation/AnimInstance.h"

ASummonNPCSkill::ASummonNPCSkill()
{
    PrimaryActorTick.bCanEverTick = true;
    
    // Create root component (capsule for the NPC)
    CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
    RootComponent = CapsuleComponent;
    CapsuleComponent->InitCapsuleSize(42.0f, 96.0f);
    CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    
    // Create NPC mesh component
    NPCMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("NPCMesh"));
    NPCMesh->SetupAttachment(RootComponent);
    NPCMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f)); // Changed from -96 to -90
    NPCMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    
    // Create detection sphere
    DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
    DetectionSphere->SetupAttachment(RootComponent);
    DetectionSphere->SetSphereRadius(AttackRadius);
    DetectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    DetectionSphere->SetCollisionObjectType(ECC_WorldDynamic);
    DetectionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    DetectionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    DetectionSphere->SetGenerateOverlapEvents(true);
}

void ASummonNPCSkill::BeginPlay()
{
    Super::BeginPlay();
    
    // Bind overlap event for debugging
    if (DetectionSphere)
    {
        DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &ASummonNPCSkill::OnOverlapBegin);
    }
    
    // Initialize the skill immediately if spawned in world
    if (GetActorLocation() != FVector::ZeroVector)
    {
        InitializeSkill(GetActorLocation());
    }
}

void ASummonNPCSkill::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    UE_LOG(LogTemp, Warning, TEXT("SummonNPCSkill: Overlap detected with %s"), 
        OtherActor ? *OtherActor->GetName() : TEXT("NULL"));
}

void ASummonNPCSkill::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    // Debug visualization
    if (bShowAttackRadius)
    {
        DrawDebugSphere(
            GetWorld(),
            GetActorLocation(),
            AttackRadius,
            32,
            FColor::Red,
            false,
            0.0f,
            0,
            2.0f
        );
    }
}

void ASummonNPCSkill::InitializeSkill(const FVector& MouseWorldLocation)
{
    // Ensure spawn location is on the ground
    FHitResult HitResult;
    FVector StartTrace = MouseWorldLocation + FVector(0, 0, 500.0f);
    FVector EndTrace = MouseWorldLocation - FVector(0, 0, 500.0f);
    
    FVector SpawnLocation = MouseWorldLocation;
    if (GetWorld()->LineTraceSingleByChannel(
        HitResult,
        StartTrace,
        EndTrace,
        ECC_Visibility))
    {
        SpawnLocation = HitResult.Location;
        // Add a small offset to ensure the NPC isn't stuck in the ground
        SpawnLocation.Z += 5.0f;
    }
    
    // Set actor location
    SetActorLocation(SpawnLocation);
    
    // Setup NPC appearance
    SetupNPCAppearance();
    
    // Update detection sphere radius to match AttackRadius
    if (DetectionSphere)
    {
        DetectionSphere->SetSphereRadius(AttackRadius);
    }
    
    // Start attack timer
    GetWorld()->GetTimerManager().SetTimer(
        AttackTimerHandle,
        this,
        &ASummonNPCSkill::AttackNearbyEnemies,
        1.0f / AttackRate,
        true,
        0.1f // Small initial delay
    );
    
    // Set lifetime timer
    GetWorld()->GetTimerManager().SetTimer(
        LifetimeTimerHandle,
        this,
        &ASummonNPCSkill::DespawnNPC,
        NPCLifetime,
        false
    );
    
    // Play idle animation only if it exists
    if (IdleAnimation && NPCMesh && NPCMesh->GetAnimInstance())
    {
        NPCMesh->GetAnimInstance()->Montage_Play(IdleAnimation, 1.0f);
    }
    
    UE_LOG(LogTemp, Warning, TEXT("SummonNPCSkill: Initialized at %s with attack radius %.1f"), 
        *SpawnLocation.ToString(), AttackRadius);
}

void ASummonNPCSkill::SetupNPCAppearance()
{
    // Set skeletal mesh
    if (NPCSkeletalMesh && NPCMesh)
    {
        NPCMesh->SetSkeletalMesh(NPCSkeletalMesh);
    }
    
    // Set animation blueprint
    if (NPCAnimBlueprint && NPCMesh)
    {
        NPCMesh->SetAnimInstanceClass(NPCAnimBlueprint);
    }
    
    // Set scale
    NPCMesh->SetRelativeScale3D(NPCScale);
}

void ASummonNPCSkill::AttackNearbyEnemies()
{
    // Get enemies in range
    EnemiesInRange = GetEnemiesInRadius();
    
    UE_LOG(LogTemp, Warning, TEXT("SummonNPCSkill: AttackNearbyEnemies called - Found %d enemies in range"), 
        EnemiesInRange.Num());
    
    if (EnemiesInRange.Num() > 0)
    {
        // Find closest enemy
        CurrentTarget = FindClosestEnemy();
        
        if (CurrentTarget)
        {
            UE_LOG(LogTemp, Warning, TEXT("SummonNPCSkill: Found target %s"), *CurrentTarget->GetName());
            
            // Rotate to face target
            RotateToTarget(CurrentTarget);
            
            // Attack immediately without animation delay
            AttackEnemy(CurrentTarget);
            
            // Play attack animation only if it exists
            if (AttackAnimation)
            {
                PlayAttackAnimation();
            }
        }
    }
}

TArray<AActor*> ASummonNPCSkill::GetEnemiesInRadius()
{
    TArray<AActor*> Enemies;
    
    // Get all overlapping actors
    TArray<AActor*> OverlappingActors;
    DetectionSphere->GetOverlappingActors(OverlappingActors);
    
    UE_LOG(LogTemp, Warning, TEXT("SummonNPCSkill: Found %d overlapping actors"), OverlappingActors.Num());
    
    // Filter for enemies
    for (AActor* Actor : OverlappingActors)
    {
        if (IsValidEnemy(Actor))
        {
            float Distance = FVector::Dist(GetActorLocation(), Actor->GetActorLocation());
            if (Distance <= AttackRadius)
            {
                Enemies.Add(Actor);
                UE_LOG(LogTemp, Warning, TEXT("SummonNPCSkill: Added enemy %s at distance %.1f"), 
                    *Actor->GetName(), Distance);
            }
        }
    }
    
    return Enemies;
}

bool ASummonNPCSkill::IsValidEnemy(AActor* Actor)
{
    if (!Actor || !IsValid(Actor))
    {
        return false;
    }
    
    // Check if it's an enemy type
    if (Cast<AEnemy>(Actor) || Cast<ABossTest>(Actor))
    {
        return true;
    }
    
    // You can add more enemy types here
    // For example: if (Actor->ActorHasTag("Enemy")) { return true; }
    
    return false;
}

AActor* ASummonNPCSkill::FindClosestEnemy()
{
    AActor* ClosestEnemy = nullptr;
    float ClosestDistance = FLT_MAX;
    
    FVector MyLocation = GetActorLocation();
    
    for (AActor* Enemy : EnemiesInRange)
    {
        if (IsValid(Enemy))
        {
            float Distance = FVector::Dist(MyLocation, Enemy->GetActorLocation());
            if (Distance < ClosestDistance)
            {
                ClosestDistance = Distance;
                ClosestEnemy = Enemy;
            }
        }
    }
    
    return ClosestEnemy;
}

void ASummonNPCSkill::AttackEnemy(AActor* Enemy)
{
    if (!Enemy || !IsValid(Enemy))
    {
        return;
    }
    
    // Draw a debug line to show attack
    if (bShowAttackRadius)
    {
        DrawDebugLine(
            GetWorld(),
            GetActorLocation() + FVector(0, 0, 50.0f),
            Enemy->GetActorLocation() + FVector(0, 0, 50.0f),
            FColor::Yellow,
            false,
            0.5f,
            0,
            5.0f
        );
    }
    
    // Special handling for boss
    ABossTest* Boss = Cast<ABossTest>(Enemy);
    if (Boss)
    {
        // Boss uses DealDamage function directly
        Boss->DealDamage(AttackDamage);
        
        UE_LOG(LogTemp, Warning, TEXT("SummonNPCSkill: Attacked Boss %s for %.1f damage"), 
            *Boss->GetName(), AttackDamage);
    }
    else
    {
        // Regular enemies use the standard damage system
        FHitResult HitResult;
        UGameplayStatics::ApplyPointDamage(
            Enemy,
            AttackDamage,
            Enemy->GetActorLocation(),
            HitResult,
            nullptr,
            nullptr,
            UDamageType::StaticClass()
        );
        
        UE_LOG(LogTemp, Warning, TEXT("SummonNPCSkill: Attacked Enemy %s for %.1f damage"), 
            *Enemy->GetName(), AttackDamage);
    }
}

void ASummonNPCSkill::RotateToTarget(AActor* Target)
{
    if (!Target)
    {
        return;
    }
    
    FVector Direction = Target->GetActorLocation() - GetActorLocation();
    Direction.Z = 0; // Keep rotation on horizontal plane
    
    if (!Direction.IsNearlyZero())
    {
        FRotator NewRotation = Direction.Rotation();
        SetActorRotation(NewRotation);
    }
}

void ASummonNPCSkill::PlayAttackAnimation()
{
    if (AttackAnimation && NPCMesh && NPCMesh->GetAnimInstance())
    {
        // Stop idle animation if playing
        if (IdleAnimation)
        {
            NPCMesh->GetAnimInstance()->Montage_Stop(0.2f, IdleAnimation);
        }
        
        // Play attack animation
        NPCMesh->GetAnimInstance()->Montage_Play(AttackAnimation, 1.0f);
        
        // Return to idle after attack
        float AttackDuration = AttackAnimation->GetPlayLength();
        FTimerHandle ReturnToIdleTimer;
        GetWorld()->GetTimerManager().SetTimer(
            ReturnToIdleTimer,
            [this]()
            {
                if (IdleAnimation && NPCMesh && NPCMesh->GetAnimInstance())
                {
                    NPCMesh->GetAnimInstance()->Montage_Play(IdleAnimation, 1.0f);
                }
            },
            AttackDuration,
            false
        );
    }
}

void ASummonNPCSkill::DespawnNPC()
{
    // Clear timers
    GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
    GetWorld()->GetTimerManager().ClearTimer(LifetimeTimerHandle);
    
    // Destroy this actor
    Destroy();
}