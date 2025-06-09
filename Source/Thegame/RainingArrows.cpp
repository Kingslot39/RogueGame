#include "RainingArrows.h"
#include "RainArrow.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

ARainingArrows::ARainingArrows()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ARainingArrows::BeginPlay()
{
    Super::BeginPlay();
    
    // Get mouse cursor location when skill starts
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC)
    {
        FHitResult HitResult;
        if (PC->GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
        {
            TargetCenter = HitResult.Location;
        }
        else
        {
            // Fallback to actor location if no hit
            TargetCenter = GetActorLocation();
        }
    }

    // Start spawning arrows
    GetWorld()->GetTimerManager().SetTimer(
        SpawnTimerHandle,
        this,
        &ARainingArrows::SpawnArrow,
        ArrowSpawnRate,
        true,
        0.0f
    );

    // Set duration timer
    GetWorld()->GetTimerManager().SetTimer(
        DurationTimerHandle,
        this,
        &ARainingArrows::OnDurationExpired,
        SkillDuration,
        false
    );
}

void ARainingArrows::SpawnArrow()
{
    if (!ArrowClass)
    {
        return;
    }

    // Get random position within radius
    float RandomAngle = FMath::RandRange(0.0f, 360.0f);
    float RandomDistance = FMath::RandRange(0.0f, SkillRadius);
    
    FVector Offset = FVector(
        FMath::Cos(FMath::DegreesToRadians(RandomAngle)) * RandomDistance,
        FMath::Sin(FMath::DegreesToRadians(RandomAngle)) * RandomDistance,
        0.0f
    );

    FVector SpawnLocation = TargetCenter + Offset + FVector(0, 0, 1500); // Spawn 1500 units above
    FRotator SpawnRotation = FRotator(-90, 0, 0); // Point downward

    // Spawn the arrow
    ARainArrow* Arrow = GetWorld()->SpawnActor<ARainArrow>(
        ArrowClass,
        SpawnLocation,
        SpawnRotation
    );

    if (Arrow)
    {
        // Set damage
        Arrow->Damage = ArrowDamage;
        
        // Set downward velocity
        if (Arrow->ProjectileMovement)
        {
            Arrow->ProjectileMovement->Velocity = FVector(0, 0, -2000);
        }
    }
}

void ARainingArrows::OnDurationExpired()
{
    // Clear timers
    GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
    GetWorld()->GetTimerManager().ClearTimer(DurationTimerHandle);

    // Destroy this actor
    Destroy();
}