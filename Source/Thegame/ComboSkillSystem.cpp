#include "ComboSkillSystem.h"
#include "WeaponSystem.h"
#include "InGameUI.h"  // Include your UI class
#include "GameFramework/Character.h"

AComboSkillSystem::AComboSkillSystem()
{
    PrimaryActorTick.bCanEverTick = true;
    
    // Create a simple root component
    USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    RootComponent = SceneRoot;
    
    // Initialize combo names
    ComboAB.ComboName = "LightShieldForce";
    ComboAC.ComboName = "LightSpearTornado";
    ComboAD.ComboName = "LightHammerQuake";
    ComboBC.ComboName = "ShieldSpearWall";
    ComboBD.ComboName = "ShieldHammerGuardian";
    ComboCD.ComboName = "SpearHammerStorm";
}

void AComboSkillSystem::BeginPlay()
{
    Super::BeginPlay();
}

void AComboSkillSystem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    // Update cooldowns for all combos
    UpdateCooldowns(DeltaTime);
}

void AComboSkillSystem::SetOwningActor(AActor* NewOwner)
{
    OwningActor = NewOwner;  // Changed Owner to NewOwner
    
    // Find the weapon system on the owner if not already set
    if (!WeaponSystem)
    {
        ACharacter* CharacterOwner = Cast<ACharacter>(NewOwner);  // Changed Owner to NewOwner
        if (CharacterOwner)
        {
            // Find the weapon system in the character's hierarchy
            TArray<AActor*> AttachedActors;
            CharacterOwner->GetAttachedActors(AttachedActors);
            
            for (AActor* AttachedActor : AttachedActors)
            {
                AWeaponSystem* FoundWeaponSystem = Cast<AWeaponSystem>(AttachedActor);
                if (FoundWeaponSystem)
                {
                    WeaponSystem = FoundWeaponSystem;
                    break;
                }
            }
        }
    }
}

void AComboSkillSystem::SetWeaponSystem(AWeaponSystem* InWeaponSystem)
{
    WeaponSystem = InWeaponSystem;
}

void AComboSkillSystem::SetInGameUI(UInGameUI* UI)
{
    InGameUI = UI;
}

bool AComboSkillSystem::TryActivateCombo()
{
    if (!WeaponSystem)
    {
        UE_LOG(LogTemp, Warning, TEXT("ComboSkillSystem: WeaponSystem reference is null!"));
        return false;
    }
    
    // Get currently selected weapons
    EWeaponType Weapon1 = WeaponSystem->ActiveWeaponSlot1;
    EWeaponType Weapon2 = WeaponSystem->ActiveWeaponSlot2;
    
    // Try to activate the combo
    bool result = ActivateCombo(Weapon1, Weapon2);
    
    // Force UI update regardless of success
    if (InGameUI)
    {
        InGameUI->UpdateValues();
    }
    
    return result;
}

FComboSkillData* AComboSkillSystem::GetComboDataPtr(EWeaponType Weapon1, EWeaponType Weapon2)
{
    // Original pointer-returning implementation
    // ... same as before ...
    // Make sure we have two different weapons
    if (Weapon1 == EWeaponType::None || Weapon2 == EWeaponType::None || Weapon1 == Weapon2)
    {
        return nullptr;
    }
    
    // Ensure weapon order is consistent (A comes before B, etc.)
    if (Weapon1 > Weapon2)
    {
        Swap(Weapon1, Weapon2);
    }
    
    // Return the appropriate combo based on weapon combination
    if (Weapon1 == EWeaponType::WeaponA && Weapon2 == EWeaponType::WeaponB)
    {
        return &ComboAB;
    }
    else if (Weapon1 == EWeaponType::WeaponA && Weapon2 == EWeaponType::WeaponC)
    {
        return &ComboAC;
    }
    else if (Weapon1 == EWeaponType::WeaponA && Weapon2 == EWeaponType::WeaponD)
    {
        return &ComboAD;
    }
    else if (Weapon1 == EWeaponType::WeaponB && Weapon2 == EWeaponType::WeaponC)
    {
        return &ComboBC;
    }
    else if (Weapon1 == EWeaponType::WeaponB && Weapon2 == EWeaponType::WeaponD)
    {
        return &ComboBD;
    }
    else if (Weapon1 == EWeaponType::WeaponC && Weapon2 == EWeaponType::WeaponD)
    {
        return &ComboCD;
    }
    
    return nullptr;
}
bool AComboSkillSystem::IsComboReady(EWeaponType Weapon1, EWeaponType Weapon2)
{
    FComboSkillData* ComboData = GetComboDataPtr(Weapon1, Weapon2);
    if (!ComboData)
    {
        return false;
    }
    
    // If cooldown is 0, the combo is ready
    return ComboData->CooldownRemaining <= 0.0f;
}

bool AComboSkillSystem::ActivateCombo(EWeaponType Weapon1, EWeaponType Weapon2)
{
    FComboSkillData* ComboData = GetComboDataPtr(Weapon1, Weapon2);
    if (!ComboData || !IsComboReady(Weapon1, Weapon2))
    {
        return false;
    }
    
    // Activate the combo
    UE_LOG(LogTemp, Display, TEXT("Activating combo %s"), *ComboData->ComboName);
    
    // Set cooldown
    ComboData->CooldownRemaining = ComboData->Cooldown;
    
    // Create combo effect
    ActivateComboEffect(*ComboData);
    
    // Broadcast that combo was activated
    OnComboActivated.Broadcast(ComboData->ComboName);
    
    // Broadcast initial cooldown state - use 0.0f since we just started the cooldown
    // (0.0f means 0% progress, which is correct when cooldown just started)
    OnComboCooldownChanged.Broadcast(ComboData->ComboName, 0.0f);
    
    // Force immediate UI update
    if (InGameUI)
    {
        InGameUI->UpdateValues();
    }
    
    return true;
}

void AComboSkillSystem::UpdateCooldowns(float DeltaTime)
{
    // Update cooldowns for all combos
    TArray<FComboSkillData*> AllCombos = { &ComboAB, &ComboAC, &ComboAD, &ComboBC, &ComboBD, &ComboCD };
    
    bool bAnyActiveCooldown = false;
    
    for (FComboSkillData* Combo : AllCombos)
    {
        if (Combo->CooldownRemaining > 0.0f)
        {
            bAnyActiveCooldown = true;
            
            // Reduce cooldown
            float PreviousCooldown = Combo->CooldownRemaining;
            Combo->CooldownRemaining = FMath::Max(0.0f, Combo->CooldownRemaining - DeltaTime);
            
            // Calculate cooldown percentage (inverted from original)
            // Now 0.0 means just started cooldown, 1.0 means cooldown finished
            float CooldownProgress = 1.0f - (Combo->CooldownRemaining / Combo->Cooldown);
            
            // Always broadcast the change for continuous UI updates
            OnComboCooldownChanged.Broadcast(Combo->ComboName, CooldownProgress);
        }
    }
    
    // If we have any active cooldowns and an InGameUI reference, force update the UI
    if (bAnyActiveCooldown && InGameUI && WeaponSystem)
    {
        // Use a static variable to track time since last update
        static float TimeSinceLastUpdate = 0.0f;
        TimeSinceLastUpdate += DeltaTime;
        
        // Update UI every 0.1 seconds while any cooldown is active
        if (TimeSinceLastUpdate >= 0.1f)
        {
            InGameUI->UpdateValues();
            TimeSinceLastUpdate = 0.0f;
        }
    }
}

FComboSkillData AComboSkillSystem::GetComboData(EWeaponType Weapon1, EWeaponType Weapon2)
{
    FComboSkillData* DataPtr = GetComboDataPtr(Weapon1, Weapon2);
    if (DataPtr)
    {
        return *DataPtr;
    }
    return FComboSkillData();
}

void AComboSkillSystem::ActivateComboEffect(FComboSkillData& ComboData)
{
    // If we have a combo effect to spawn and an owner, spawn the effect
    if (ComboData.ComboEffect && OwningActor)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = OwningActor;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        
        // Spawn the combo effect at the owner's location
        AActor* SpawnedEffect = GetWorld()->SpawnActor<AActor>(
            ComboData.ComboEffect, 
            OwningActor->GetActorLocation(),
            OwningActor->GetActorRotation(),
            SpawnParams
        );
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot activate combo effect: Missing effect class or owner!"));
    }
}

FString AComboSkillSystem::GetComboNameForWeapons(EWeaponType Weapon1, EWeaponType Weapon2)
{
    // Make sure we have two different weapons
    if (Weapon1 == EWeaponType::None || Weapon2 == EWeaponType::None || Weapon1 == Weapon2)
    {
        return "Invalid";
    }
    
    // Ensure weapon order is consistent (A comes before B, etc.)
    if (Weapon1 > Weapon2)
    {
        Swap(Weapon1, Weapon2);
    }
    
    // Return string identifier based on the weapon combination
    if (Weapon1 == EWeaponType::WeaponA && Weapon2 == EWeaponType::WeaponB)
    {
        return "AB";
    }
    else if (Weapon1 == EWeaponType::WeaponA && Weapon2 == EWeaponType::WeaponC)
    {
        return "AC";
    }
    else if (Weapon1 == EWeaponType::WeaponA && Weapon2 == EWeaponType::WeaponD)
    {
        return "AD";
    }
    else if (Weapon1 == EWeaponType::WeaponB && Weapon2 == EWeaponType::WeaponC)
    {
        return "BC";
    }
    else if (Weapon1 == EWeaponType::WeaponB && Weapon2 == EWeaponType::WeaponD)
    {
        return "BD";
    }
    else if (Weapon1 == EWeaponType::WeaponC && Weapon2 == EWeaponType::WeaponD)
    {
        return "CD";
    }
    
    return "Unknown";
}
float AComboSkillSystem::GetComboCooldownProgress(EWeaponType Weapon1, EWeaponType Weapon2)
{
    FComboSkillData* ComboData = GetComboDataPtr(Weapon1, Weapon2);
    if (!ComboData)
    {
        return 1.0f;  // No combo = fully ready
    }
    
    // Calculate progress - 0.0 means just started cooldown, 1.0 means ready to use
    if (ComboData->Cooldown <= 0.0f)
    {
        return 1.0f;  // Avoid division by zero
    }
    
    float progress = 1.0f - (ComboData->CooldownRemaining / ComboData->Cooldown);
    
    // Log the progress value for debugging
    UE_LOG(LogTemp, Verbose, TEXT("Combo %s cooldown progress: %.2f (Remaining: %.2f / Total: %.2f)"),
           *ComboData->ComboName, progress, ComboData->CooldownRemaining, ComboData->Cooldown);
    
    return progress;
}