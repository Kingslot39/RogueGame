// WeaponSystem.cpp
#include "WeaponSystem.h"

#include "ThegameCharacter.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"

AWeaponSystem::AWeaponSystem()
{
    PrimaryActorTick.bCanEverTick = true;
    
    // Create a scene component as root
    USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    RootComponent = SceneRoot;

    // Create weapon meshes as StaticMeshComponent - ensure they're properly created
    WeaponMesh1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh1"));
    if (WeaponMesh1)
    {
        WeaponMesh1->SetupAttachment(RootComponent);
        WeaponMesh1->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        WeaponMesh1->SetVisibility(false);
    }

    WeaponMesh2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh2"));
    if (WeaponMesh2)
    {
        WeaponMesh2->SetupAttachment(RootComponent);
        WeaponMesh2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        WeaponMesh2->SetVisibility(false);
    }
    WeaponMesh3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh3"));
    if(WeaponMesh3)
    {
        WeaponMesh3->SetupAttachment(RootComponent);
        WeaponMesh3->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        WeaponMesh3->SetVisibility(false);
    }
    
    // Initialize weapon slots to None
    ActiveWeaponSlot1 = EWeaponType::None;
    ActiveWeaponSlot2 = EWeaponType::None;
    ActiveWeaponSlot3 = EWeaponType::None;
}

void AWeaponSystem::BeginPlay()
{
    Super::BeginPlay();
    
    // Hide meshes initially - we'll show them only when properly initialized
    if (WeaponMesh1)
    {
        WeaponMesh1->SetVisibility(false);
    }
    
    if (WeaponMesh2)
    {
        WeaponMesh2->SetVisibility(false);
    }
    if (WeaponMesh3)
    {
        WeaponMesh3->SetVisibility(false);
    }
    
    UE_LOG(LogTemp, Warning, TEXT("WeaponSystem BeginPlay - ActiveWeaponSlot1: %d, ActiveWeaponSlot2: %d"), 
           (int32)ActiveWeaponSlot1, (int32)ActiveWeaponSlot2);
}

void AWeaponSystem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    // Add any per-frame logic here
}

void AWeaponSystem::SetOwningActor(AActor* NewOwner)
{
    if (!NewOwner)
    {
        UE_LOG(LogTemp, Error, TEXT("SetOwningActor called with NULL owner!"));
        return;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("SetOwningActor called with owner: %s"), *NewOwner->GetName());
    
    // Store the owner
    OwningActor = NewOwner;
    
    // Log weapon data to help with debugging
    LogWeaponDataStatus();
}

void AWeaponSystem::LogWeaponDataStatus()
{
    // Check and log if each weapon type has a valid mesh assigned
    UE_LOG(LogTemp, Warning, TEXT("WeaponA has mesh: %s"), WeaponA.WeaponMesh ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogTemp, Warning, TEXT("WeaponB has mesh: %s"), WeaponB.WeaponMesh ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogTemp, Warning, TEXT("WeaponC has mesh: %s"), WeaponC.WeaponMesh ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogTemp, Warning, TEXT("WeaponD has mesh: %s"), WeaponD.WeaponMesh ? TEXT("YES") : TEXT("NO"));
}

FWeaponData AWeaponSystem::GetWeaponData(EWeaponType WeaponType) const
{
    // Direct access to each weapon's data
    switch (WeaponType)
    {
        case EWeaponType::WeaponA:
            return WeaponA;
        case EWeaponType::WeaponB:
            return WeaponB;
        case EWeaponType::WeaponC:
            return WeaponC;
        case EWeaponType::WeaponD:
            return WeaponD;
        default:
            return FWeaponData();
    }
}

bool AWeaponSystem::IsWeaponTypeValid(EWeaponType WeaponType) const
{
    // Check if a weapon type has a valid mesh
    FWeaponData Data = GetWeaponData(WeaponType);
    return Data.WeaponMesh != nullptr;
}

bool AWeaponSystem::IsAttackWeapon(EWeaponType WeaponType) const
{
    // A and C are attack weapons
    return WeaponType == EWeaponType::WeaponA || WeaponType == EWeaponType::WeaponC;
}

bool AWeaponSystem::IsSupportWeapon(EWeaponType WeaponType) const
{
    // B and D are support weapons
    return WeaponType == EWeaponType::WeaponB || WeaponType == EWeaponType::WeaponD;
}

void AWeaponSystem::SelectWeapons(EWeaponType Weapon1, EWeaponType Weapon2)
{
    UE_LOG(LogTemp, Warning, TEXT("SelectWeapons called: Weapon1=%d, Weapon2=%d"), (int32)Weapon1, (int32)Weapon2);
    
    // Store previous weapons for comparison
    EWeaponType PreviousWeapon1 = ActiveWeaponSlot1;
    EWeaponType PreviousWeapon2 = ActiveWeaponSlot2;
    
    // First, handle simple cases: None or just one weapon
    if (Weapon1 == EWeaponType::None && Weapon2 == EWeaponType::None)
    {
        // Both weapons are None - clear both slots
        ActiveWeaponSlot1 = EWeaponType::None;
        ActiveWeaponSlot2 = EWeaponType::None;
    }
    else if (Weapon1 != EWeaponType::None && Weapon2 == EWeaponType::None)
    {
        // Only Weapon1 is set
        if (IsAttackWeapon(Weapon1))
        {
            ActiveWeaponSlot1 = Weapon1;
            ActiveWeaponSlot2 = EWeaponType::None;
        }
        else if (IsSupportWeapon(Weapon1))
        {
            ActiveWeaponSlot1 = EWeaponType::None;
            ActiveWeaponSlot2 = Weapon1;
        }
        else
        {
            // Invalid weapon type, don't change anything
            UE_LOG(LogTemp, Warning, TEXT("Invalid weapon type for Weapon1: %d"), (int32)Weapon1);
            return;
        }
    }
    else if (Weapon1 == EWeaponType::None && Weapon2 != EWeaponType::None)
    {
        // Only Weapon2 is set
        if (IsAttackWeapon(Weapon2))
        {
            ActiveWeaponSlot1 = Weapon2;
            ActiveWeaponSlot2 = EWeaponType::None;
        }
        else if (IsSupportWeapon(Weapon2))
        {
            ActiveWeaponSlot1 = EWeaponType::None;
            ActiveWeaponSlot2 = Weapon2;
        }
        else
        {
            // Invalid weapon type, don't change anything
            UE_LOG(LogTemp, Warning, TEXT("Invalid weapon type for Weapon2: %d"), (int32)Weapon2);
            return;
        }
    }
    else if (Weapon1 == Weapon2 && Weapon1 != EWeaponType::None)
    {
        // Both weapons are the same and not None
        UE_LOG(LogTemp, Warning, TEXT("Cannot select the same weapon in both slots!"));
        return;
    }
    else
    {
        // Both weapons are set and different
        bool bIsAttack1 = IsAttackWeapon(Weapon1);
        bool bIsAttack2 = IsAttackWeapon(Weapon2);
        bool bIsSupport1 = IsSupportWeapon(Weapon1);
        bool bIsSupport2 = IsSupportWeapon(Weapon2);
        
        if (bIsAttack1 && bIsSupport2)
        {
            // Attack weapon in Slot1, Support weapon in Slot2 - perfect case
            ActiveWeaponSlot1 = Weapon1;
            ActiveWeaponSlot2 = Weapon2;
        }
        else if (bIsSupport1 && bIsAttack2)
        {
            // Support weapon in Slot1, Attack weapon in Slot2 - swap them
            ActiveWeaponSlot1 = Weapon2; // Attack weapon goes to Slot1
            ActiveWeaponSlot2 = Weapon1; // Support weapon goes to Slot2
            UE_LOG(LogTemp, Warning, TEXT("Weapons swapped for consistent attack/support slots"));
        }
        else if (bIsAttack1 && bIsAttack2)
        {
            // Both are attack weapons - use the first one and leave Slot2 empty
            ActiveWeaponSlot1 = Weapon1;
            ActiveWeaponSlot2 = EWeaponType::None;
            UE_LOG(LogTemp, Warning, TEXT("Two attack weapons provided, using only the first one"));
        }
        else if (bIsSupport1 && bIsSupport2)
        {
            // Both are support weapons - put the first one in Slot2 and leave Slot1 empty
            ActiveWeaponSlot1 = EWeaponType::None;
            ActiveWeaponSlot2 = Weapon1;
            UE_LOG(LogTemp, Warning, TEXT("Two support weapons provided, using only the first one"));
        }
        else
        {
            // Invalid weapon types, don't change anything
            UE_LOG(LogTemp, Warning, TEXT("Invalid weapon combination: Weapon1=%d, Weapon2=%d"), 
                (int32)Weapon1, (int32)Weapon2);
            return;
        }
    }
    
    // Only update weapon visuals if we have an owner
    if (OwningActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("SelectWeapons: Calling UpdateWeaponMeshes with valid owner"));
        UpdateWeaponMeshes();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("SelectWeapons: Skipping UpdateWeaponMeshes because owner is null"));
    }
    
    // If weapons changed, broadcast the change
    if (PreviousWeapon1 != ActiveWeaponSlot1 || PreviousWeapon2 != ActiveWeaponSlot2)
    {
        OnWeaponsChanged.Broadcast(ActiveWeaponSlot1, ActiveWeaponSlot2);
        // Apply weapon buffs when weapons change
        ApplyWeaponBuffs();
    }
}

void AWeaponSystem::UpdateWeaponMeshes()
{
    // Create meshes if they don't exist
    if (!WeaponMesh1)
    {
        UE_LOG(LogTemp, Warning, TEXT("Creating WeaponMesh1 dynamically"));
        WeaponMesh1 = NewObject<UStaticMeshComponent>(this, TEXT("WeaponMesh1"));
        if (WeaponMesh1)
        {
            WeaponMesh1->RegisterComponent();
            WeaponMesh1->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
            WeaponMesh1->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }
    }
    
    if (!WeaponMesh2)
    {
        UE_LOG(LogTemp, Warning, TEXT("Creating WeaponMesh2 dynamically"));
        WeaponMesh2 = NewObject<UStaticMeshComponent>(this, TEXT("WeaponMesh2"));
        if (WeaponMesh2)
        {
            WeaponMesh2->RegisterComponent();
            WeaponMesh2->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
            WeaponMesh2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }
    }
    
    if (!WeaponMesh1 || !WeaponMesh2) 
    {
        UE_LOG(LogTemp, Error, TEXT("UpdateWeaponMeshes: Failed to create weapon mesh components"));
        return;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("UpdateWeaponMeshes called - ActiveWeaponSlot1: %d, ActiveWeaponSlot2: %d"), 
         (int32)ActiveWeaponSlot1, (int32)ActiveWeaponSlot2);
    
    // Update first weapon mesh (attack weapon)
    if (ActiveWeaponSlot1 != EWeaponType::None)
    {
        FWeaponData WeaponData = GetWeaponData(ActiveWeaponSlot1);
        if (WeaponData.WeaponMesh)
        {
            UE_LOG(LogTemp, Warning, TEXT("Setting mesh for Attack Weapon (%d)"), (int32)ActiveWeaponSlot1);
            WeaponMesh1->SetStaticMesh(WeaponData.WeaponMesh);
            WeaponMesh1->SetVisibility(true);
            WeaponMesh1->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("No mesh assigned for weapon type %d in slot 1"), (int32)ActiveWeaponSlot1);
            WeaponMesh1->SetVisibility(false);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No weapon in slot 1"));
        WeaponMesh1->SetVisibility(false);
    }

    // Update second weapon mesh (support weapon)
    if (ActiveWeaponSlot2 != EWeaponType::None)
    {
        FWeaponData WeaponData = GetWeaponData(ActiveWeaponSlot2);
        if (WeaponData.WeaponMesh)
        {
            UE_LOG(LogTemp, Warning, TEXT("Setting mesh for Support Weapon (%d)"), (int32)ActiveWeaponSlot2);
            WeaponMesh2->SetStaticMesh(WeaponData.WeaponMesh);
            WeaponMesh2->SetVisibility(true);
            WeaponMesh2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("No mesh assigned for weapon type %d in slot 2"), (int32)ActiveWeaponSlot2);
            WeaponMesh2->SetVisibility(false);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No weapon in slot 2"));
        WeaponMesh2->SetVisibility(false);
    }
    
    // Critical: Update weapon attachments after changing meshes
    FTimerHandle AttachmentTimerHandle;
    GetWorld()->GetTimerManager().SetTimer(
        AttachmentTimerHandle, 
        [this]()
        {
            if (OwningActor)
            {
                UE_LOG(LogTemp, Warning, TEXT("Calling UpdateWeaponAttachments with delay"));
                UpdateWeaponAttachments();
            }
        }, 
        0.2f, // Small delay to ensure meshes are ready
        false
    );
}

void AWeaponSystem::UpdateWeaponAttachments()
{
    // Check if we have a valid owner before trying to access it
    if (!OwningActor) 
    {
        UE_LOG(LogTemp, Warning, TEXT("UpdateWeaponAttachments called with null OwningActor"));
        return;
    }
    
    ACharacter* CharacterOwner = Cast<ACharacter>(OwningActor);
    if (!CharacterOwner) 
    {
        UE_LOG(LogTemp, Warning, TEXT("OwningActor is not a Character"));
        return;
    }
    
    USkeletalMeshComponent* CharMesh = CharacterOwner->GetMesh();
    if (!CharMesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("Character does not have a valid mesh"));
        return;
    }
    
    // Check if the required sockets exist
    FName SwordSocket = FName("Sword_Righthand");  // For attack weapons (A/C)
    FName ShieldSocket = FName("Shield_Socket");   // For support weapons (B/D)
    
    bool hasSwordSocket = CharMesh->DoesSocketExist(SwordSocket);
    bool hasShieldSocket = CharMesh->DoesSocketExist(ShieldSocket);
    
    UE_LOG(LogTemp, Warning, TEXT("Socket check - Sword_Righthand: %s, Shield_Socket: %s"),
           hasSwordSocket ? TEXT("EXISTS") : TEXT("MISSING"),
           hasShieldSocket ? TEXT("EXISTS") : TEXT("MISSING"));
    
    if (!hasSwordSocket && !hasShieldSocket) {
        UE_LOG(LogTemp, Error, TEXT("Character mesh has no weapon sockets! Weapon attachment failed."));
        return;
    }
    
    // Handle attack weapon (A/C)
    if (ActiveWeaponSlot1 != EWeaponType::None && IsAttackWeapon(ActiveWeaponSlot1))
    {
        if (WeaponMesh1 && WeaponMesh1->GetStaticMesh() && hasSwordSocket)
        {
            // Detach first to avoid conflicts
            WeaponMesh1->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
            
            // Now attach
            WeaponMesh1->AttachToComponent(CharMesh, 
                FAttachmentTransformRules::SnapToTargetNotIncludingScale, 
                SwordSocket);
                
            // Ensure visibility is on
            WeaponMesh1->SetVisibility(true);
                
            UE_LOG(LogTemp, Warning, TEXT("Attack weapon attached to %s socket"), 
                *SwordSocket.ToString());
        }
        else if (!hasSwordSocket)
        {
            UE_LOG(LogTemp, Error, TEXT("Cannot attach attack weapon - sword socket missing"));
        }
        else if (!WeaponMesh1->GetStaticMesh())
        {
            UE_LOG(LogTemp, Error, TEXT("Cannot attach attack weapon - no mesh assigned"));
        }
    }
    
    // Handle support weapon (B/D)
    if (ActiveWeaponSlot2 != EWeaponType::None && IsSupportWeapon(ActiveWeaponSlot2))
    {
        if (WeaponMesh2 && WeaponMesh2->GetStaticMesh() && hasShieldSocket)
        {
            // Detach first to avoid conflicts
            WeaponMesh2->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
            
            // Now attach
            WeaponMesh2->AttachToComponent(CharMesh, 
                FAttachmentTransformRules::SnapToTargetNotIncludingScale, 
                ShieldSocket);
                
            // Ensure visibility is on
            WeaponMesh2->SetVisibility(true);
                
            UE_LOG(LogTemp, Warning, TEXT("Support weapon attached to %s socket"), 
                *ShieldSocket.ToString());
        }
        else if (!hasShieldSocket)
        {
            UE_LOG(LogTemp, Error, TEXT("Cannot attach support weapon - shield socket missing"));
        }
        else if (!WeaponMesh2->GetStaticMesh())
        {
            UE_LOG(LogTemp, Error, TEXT("Cannot attach support weapon - no mesh assigned"));
        }
    }
    
    // Final check
    UE_LOG(LogTemp, Warning, TEXT("UpdateWeaponAttachments complete - ActiveWeaponSlot1: %d, ActiveWeaponSlot2: %d"), 
        (int32)ActiveWeaponSlot1, (int32)ActiveWeaponSlot2);
}

// Complete revised implementation of ApplyWeaponBuffs and ClearWeaponBuffs

void AWeaponSystem::ApplyWeaponBuffs()
{
    // First, clear any existing buffs to avoid stacking
    ClearWeaponBuffs();
    
    // Get the character owner
    AThegameCharacter* CharacterOwner = Cast<AThegameCharacter>(OwningActor);
    if (!CharacterOwner)
    {
        UE_LOG(LogTemp, Warning, TEXT("ApplyWeaponBuffs: No valid character owner!"));
        return;
    }
    
    // Initialize buff values
    float DamageMultiplier = 1.0f;
    float HealthBonus = 0.0f;
    float SpeedMultiplier = 1.0f;
    
    // Check for attack weapon (slot 1) buffs
    if (ActiveWeaponSlot1 != EWeaponType::None)
    {
        switch (ActiveWeaponSlot1)
        {
            case EWeaponType::WeaponA: // Sword
                // Increase damage by 30%
                DamageMultiplier = 1.3f;
                UE_LOG(LogTemp, Display, TEXT("Applying WeaponA buff: +30%% Damage"));
                break;
                
            case EWeaponType::WeaponC:
                // Increase damage by 20%
                DamageMultiplier = 1.2f;
                UE_LOG(LogTemp, Display, TEXT("Applying WeaponC buff: +20%% Damage"));
                break;
                
            default:
                break;
        }
    }
    
    // Check for support weapon (slot 2) buffs
    if (ActiveWeaponSlot2 != EWeaponType::None)
    {
        switch (ActiveWeaponSlot2)
        {
            case EWeaponType::WeaponB: // Shield
                // Increase health by 40
                HealthBonus += 40.0f;
                UE_LOG(LogTemp, Display, TEXT("Applying WeaponB buff: +40 Health"));
                break;
                
            case EWeaponType::WeaponD:
                // Increase speed by 10% and health by 20
                SpeedMultiplier = 1.1f; 
                HealthBonus += 20.0f;
                UE_LOG(LogTemp, Display, TEXT("Applying WeaponD buff: +10%% Speed, +20 Health"));
                break;
                
            default:
                break;
        }
    }
    
    // Apply the calculated buffs
    
    // 1. Apply damage multiplier - update both base damage and current damage
    CharacterOwner->BaseDamageAmount *= DamageMultiplier;
    CharacterOwner->DamageAmount = CharacterOwner->BaseDamageAmount;
    
    // 2. Apply health bonus
    if (HealthBonus > 0.0f)
    {
        float OldMaxHealth = CharacterOwner->MaxHealth;
        CharacterOwner->MaxHealth = CharacterOwner->BaseMaxHealth + HealthBonus;
        
        // Also increase current health proportionally
        float HealthRatio = CharacterOwner->CurrentHealth / OldMaxHealth;
        CharacterOwner->CurrentHealth = FMath::Min(
            CharacterOwner->CurrentHealth + HealthBonus, 
            CharacterOwner->MaxHealth
        );
    }
    
    // 3. Apply speed multiplier
    CharacterOwner->SpeedMultiplier = SpeedMultiplier;
    
    // Store the applied buffs for reference
    CurrentDamageMultiplier = DamageMultiplier;
    CurrentHealthBonus = HealthBonus;
    CurrentSpeedMultiplier = SpeedMultiplier;
    
    // Update the UI to reflect changes
    if (CharacterOwner->InGameUI)
    {
        CharacterOwner->InGameUI->UpdateValues();
    }
}

void AWeaponSystem::ClearWeaponBuffs()
{
    AThegameCharacter* CharacterOwner = Cast<AThegameCharacter>(OwningActor);
    if (!CharacterOwner)
    {
        return;
    }
    
    // Reset damage to base value
    CharacterOwner->DamageAmount = CharacterOwner->BaseDamageAmount;
    
    // Reset health (carefully, to avoid killing the player)
    float HealthPercentage = CharacterOwner->CurrentHealth / CharacterOwner->MaxHealth;
    CharacterOwner->MaxHealth = CharacterOwner->BaseMaxHealth;
    CharacterOwner->CurrentHealth = FMath::Max(1.0f, CharacterOwner->MaxHealth * HealthPercentage);
    
    // Reset speed multiplier
    CharacterOwner->SpeedMultiplier = 1.0f;
    
    // Reset stored buff values
    CurrentDamageMultiplier = 1.0f;
    CurrentHealthBonus = 0.0f;
    CurrentSpeedMultiplier = 1.0f;
    
    UE_LOG(LogTemp, Display, TEXT("Weapon buffs cleared, stats reset to base values"));
}