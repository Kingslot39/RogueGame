// WeaponHitDetector.cpp
#include "WeaponHitDetector.h"
#include "WeaponSystem.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Controller.h"
#include "GameFramework/DamageType.h"
//#include "Engine/DamageEvents.h" // Add this for complete FDamageEvent definition

AWeaponHitDetector::AWeaponHitDetector()
{
    PrimaryActorTick.bCanEverTick = true;
    
    // Initialize references
    WeaponSystem = nullptr;
    OwningCharacter = nullptr;
    bIsSwinging = false;
}

void AWeaponHitDetector::BeginPlay()
{
    Super::BeginPlay();
}

void AWeaponHitDetector::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AWeaponHitDetector::SetWeaponSystem(AWeaponSystem* NewWeaponSystem)
{
    WeaponSystem = NewWeaponSystem;
}

void AWeaponHitDetector::SetOwningCharacter(ACharacter* NewOwner)
{
    OwningCharacter = NewOwner;
}

void AWeaponHitDetector::StartSwing()
{
    if (!WeaponSystem || !OwningCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot start weapon swing - WeaponSystem or Owner not set"));
        return;
    }
    
    // Check if already swinging
    if (bIsSwinging)
    {
        return;
    }
    
    // Clear previously hit actors
    AlreadyHitActors.Empty();
    
    // Set swinging flag
    bIsSwinging = true;
    
    // Store initial weapon transform
    if (WeaponSystem->IsAttackWeapon(WeaponSystem->ActiveWeaponSlot1) && WeaponSystem->WeaponMesh1)
    {
        PrevWeaponTransform = WeaponSystem->WeaponMesh1->GetComponentTransform();
    }
    else if (WeaponSystem->IsAttackWeapon(WeaponSystem->ActiveWeaponSlot2) && WeaponSystem->WeaponMesh2)
    {
        PrevWeaponTransform = WeaponSystem->WeaponMesh2->GetComponentTransform();
    }
    
    // Start trace timer
    GetWorld()->GetTimerManager().SetTimer(
        TraceTimerHandle,
        this,
        &AWeaponHitDetector::PerformWeaponTrace,
        TraceFrequency,
        true
    );
    
    UE_LOG(LogTemp, Verbose, TEXT("Weapon swing started"));
}

void AWeaponHitDetector::EndSwing()
{
    // Clear timer
    GetWorld()->GetTimerManager().ClearTimer(TraceTimerHandle);
    
    // Final trace to catch any missed hits
    PerformWeaponTrace();
    
    // Reset swing state
    bIsSwinging = false;
    
    // Clear hit actors list
    AlreadyHitActors.Empty();
    
    UE_LOG(LogTemp, Verbose, TEXT("Weapon swing ended"));
}

void AWeaponHitDetector::PerformWeaponTrace()
{
    if (!WeaponSystem || !OwningCharacter || !bIsSwinging)
    {
        return;
    }
    
    // Determine which weapon mesh to use based on active slot
    UStaticMeshComponent* WeaponMesh = nullptr;
    EWeaponType ActiveWeaponType = EWeaponType::None;
    
    if (WeaponSystem->IsAttackWeapon(WeaponSystem->ActiveWeaponSlot1))
    {
        WeaponMesh = WeaponSystem->WeaponMesh1;
        ActiveWeaponType = WeaponSystem->ActiveWeaponSlot1;
    }
    else if (WeaponSystem->IsAttackWeapon(WeaponSystem->ActiveWeaponSlot2))
    {
        WeaponMesh = WeaponSystem->WeaponMesh2;
        ActiveWeaponType = WeaponSystem->ActiveWeaponSlot2;
    }
    
    if (!WeaponMesh || !WeaponMesh->GetStaticMesh())
    {
        return;
    }
    
    // Get current weapon transform
    FTransform CurrentTransform = WeaponMesh->GetComponentTransform();
    
    // Get the weapon base position (hand) and tip
    USkeletalMeshComponent* CharacterMesh = OwningCharacter->GetMesh();
    if (!CharacterMesh)
    {
        return;
    }
    
    // Get current position of the weapon tip
    FVector WeaponBase = CurrentTransform.GetLocation();
    
    // Calculate tip by using the mesh bounds
    FBoxSphereBounds Bounds = WeaponMesh->GetStaticMesh()->GetBounds();
    FVector WeaponExtent = Bounds.BoxExtent;
    
    // Estimate the tip as being ahead of the weapon in its forward direction
    FVector WeaponForward = CurrentTransform.GetRotation().GetForwardVector();
    FVector WeaponTip = WeaponBase + (WeaponForward * WeaponExtent.X * 2.0f);
    
    // Get the previous weapon positions
    FVector PrevBase = PrevWeaponTransform.GetLocation();
    FVector PrevForward = PrevWeaponTransform.GetRotation().GetForwardVector();
    FVector PrevTip = PrevBase + (PrevForward * WeaponExtent.X * 2.0f);
    
    // Set up trace parameters
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(OwningCharacter);
    QueryParams.AddIgnoredActor(this);
    QueryParams.AddIgnoredActor(WeaponSystem);
    
    // Trace from previous to current base position
    FHitResult BaseHitResult;
    bool bBaseHit = GetWorld()->SweepSingleByChannel(
    BaseHitResult,
    PrevBase,
    WeaponBase,
    FQuat::Identity,
    ECC_Pawn,  // This is the channel
    FCollisionShape::MakeSphere(TraceRadius),
    QueryParams
);
    
    // Trace from previous to current tip position
    FHitResult TipHitResult;
    bool bTipHit = GetWorld()->SweepSingleByChannel(
        TipHitResult,
        PrevTip,
        WeaponTip,
        FQuat::Identity,
        ECC_Pawn,
        FCollisionShape::MakeSphere(TraceRadius),
        QueryParams
    );
    
    // Draw debug lines if enabled
    if (bDrawDebug)
    {
        DrawDebugLine(GetWorld(), PrevBase, WeaponBase, FColor::Red, false, TraceFrequency * 2.0f);
        DrawDebugLine(GetWorld(), PrevTip, WeaponTip, FColor::Blue, false, TraceFrequency * 2.0f);
        
        if (bBaseHit)
        {
            DrawDebugSphere(GetWorld(), BaseHitResult.ImpactPoint, 10.0f, 8, FColor::Green, false, 0.5f);
        }
        
        if (bTipHit)
        {
            DrawDebugSphere(GetWorld(), TipHitResult.ImpactPoint, 10.0f, 8, FColor::Yellow, false, 0.5f);
        }
    }
    
    // Process hits
    if (bBaseHit)
    {
        ProcessHit(BaseHitResult);
    }
    
    if (bTipHit)
    {
        ProcessHit(TipHitResult);
    }
    
    // Store current transform for next trace
    PrevWeaponTransform = CurrentTransform;
}

void AWeaponHitDetector::ProcessHit(const FHitResult& HitResult)
{
    // Get hit actor
    AActor* HitActor = HitResult.GetActor();
    if (!HitActor || !WeaponSystem || !OwningCharacter)
    {
        return;
    }
    
    // Skip if already hit this actor during this swing
    if (AlreadyHitActors.Contains(HitActor))
    {
        return;
    }
    
    // Add to hit actors list
    AlreadyHitActors.Add(HitActor);
    
    // Get the active weapon type and damage
    EWeaponType ActiveWeapon = EWeaponType::None;
    float Damage = 0.0f;
    
    if (WeaponSystem->IsAttackWeapon(WeaponSystem->ActiveWeaponSlot1))
    {
        ActiveWeapon = WeaponSystem->ActiveWeaponSlot1;
        Damage = WeaponSystem->GetWeaponData(ActiveWeapon).Damage;
    }
    else if (WeaponSystem->IsAttackWeapon(WeaponSystem->ActiveWeaponSlot2))
    {
        ActiveWeapon = WeaponSystem->ActiveWeaponSlot2;
        Damage = WeaponSystem->GetWeaponData(ActiveWeapon).Damage;
    }
    
    // Apply damage
    if (ActiveWeapon != EWeaponType::None && Damage > 0.0f)
    {
        AController* OwnerController = OwningCharacter->GetController();
        
        // Debug logs to track hit detection
        UE_LOG(LogTemp, Warning, TEXT("ProcessHit: Hit Actor=%s, Damage=%.1f"), 
               *HitActor->GetName(), Damage);
        UE_LOG(LogTemp, Warning, TEXT("Hit Actor Class: %s"), *HitActor->GetClass()->GetName());
               
        // Apply damage using gameplay statics
        UE_LOG(LogTemp, Warning, TEXT("Applying damage to %s: %.1f damage"), 
               *HitActor->GetName(), Damage);
        UGameplayStatics::ApplyDamage(HitActor, Damage, OwnerController, OwningCharacter, nullptr);
        UE_LOG(LogTemp, Warning, TEXT("ApplyDamage called"));
        
        UE_LOG(LogTemp, Verbose, TEXT("Weapon hit %s for %.1f damage"), *HitActor->GetName(), Damage);
    }
    
    // Apply physics impulse if the component simulates physics
    UPrimitiveComponent* HitComponent = HitResult.GetComponent();
    if (HitComponent && HitComponent->IsSimulatingPhysics())
    {
        FVector ImpulseDir = (HitResult.ImpactPoint - HitResult.TraceStart).GetSafeNormal();
        float ImpulseStrength = 1000.0f;
        HitComponent->AddImpulseAtLocation(ImpulseDir * ImpulseStrength, HitResult.ImpactPoint);
    }
}