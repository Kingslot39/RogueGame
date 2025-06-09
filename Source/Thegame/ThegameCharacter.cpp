// Copyright Epic Games, Inc. All Rights Reserved.

#include "ThegameCharacter.h"
#include "CrossbowArrow.h"
#include "BossTest.h"
#include "Enemy.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InGameSetting.h"
#include "InputActionValue.h"
#include "WeaponHitDetector.h"
#include "RotatingShield.h"
#include "MyGameInstance.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// Constructor - Begin Play
AThegameCharacter::AThegameCharacter()
{
    // Set size for collision capsule
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
        
    // Don't rotate when the controller rotates. Let that just affect the camera.
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;
    CanDashing = true;

    // Configure character movement
    GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...    
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

    // Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
    // instead of recompiling to adjust them
    GetCharacterMovement()->JumpZVelocity = 700.f;
    GetCharacterMovement()->AirControl = 0.35f;
    GetCharacterMovement()->MaxWalkSpeed = 700.f;
    GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
    GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
    GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

    // Create a camera boom (pulls in towards the player if there is a collision)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character    
    CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

    // Create a follow camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
    FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

    // Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
    // are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
    MaxHealth = 100.0f; // Set max health
    CurrentHealth = MaxHealth; // Initialize current health to max health
	BaseMaxHealth = MaxHealth; // Initialize base max health
	// Add damage initialization
	BaseDamageAmount = 50.0f; // Base damage amount for weapons/skills
	this->DamageAmount = BaseDamageAmount; // Use 'this->' to avoid name conflict
    SwordMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sword Mesh"));
    SwordMesh->SetupAttachment(GetMesh(), FName("Sword_RightHand"));
    // Initialize weapon system and combo system pointers to nullptr
    WeaponSystem = nullptr;
    ComboSkillSystem = nullptr;

    // Initialize base skill cooldown time and skill damage
    BaseSkillCooldownTime = 3.0f; // Default cooldown time
    SkillDamage = 50.0f;          // Default skill damage

    // Initialize only if no value is set in the Blueprint
    if (BaseSkill2CooldownTime <= 0.0f)
    {
        BaseSkill2CooldownTime = 2.0f; // Default fallback
    }

    // Initialize Skill2CooldownTime based on BaseSkill2CooldownTime
    Skill2CooldownTime = BaseSkill2CooldownTime;
    UE_LOG(LogTemp, Log, TEXT("Constructor: BaseSkill2CooldownTime: %.2f"), BaseSkill2CooldownTime);
    UE_LOG(LogTemp, Log, TEXT("Constructor: Skill2CooldownTime: %.2f"), Skill2CooldownTime);
    
    // For skill combine
    // Initialize combine skill cooldown
    CombineCooldownTime = 5.0f;
    bIsSkillComReady = true;
    SkillCombineCooldownProgress = 1.0f;
    
    //Pause
    bIsPaused = false;

    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    if (PlayerController)
    {
        PlayerController->bShowMouseCursor = true;
        PlayerController->bEnableClickEvents = true;
        PlayerController->bEnableMouseOverEvents = true;
    }
    
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
}

void AThegameCharacter::BeginPlay()
{
	
    Skill2CooldownTime = BaseSkill2CooldownTime;
    SkillCooldownProgress = 1.0f;
    bIsSkillReady = true;
    
    UCharacterMovementComponent* MovementComp = GetCharacterMovement();
    if (MovementComp)
    {
        BaseSpeed = MovementComp->MaxWalkSpeed;
        SpeedMultiplier = 1.0f;
    }
    
    // Call the base class  
    Super::BeginPlay();
    
	// Create UI
	InGameUI = Cast<UInGameUI>(CreateWidget(GetGameInstance(), InGameUIClass));
	if (InGameUI)
	{
		InGameUI->Player = this;
		InGameUI->UpdateValues();
        
		// By default, show the UI (it will be hidden by dialogue if needed)
		InGameUI->AddToViewport();
        
		// Add debugging
		UE_LOG(LogTemp, Log, TEXT("Player UI created and added to viewport in BeginPlay"));
	}
    
    // Make sure weapon classes are correctly set
    UE_LOG(LogTemp, Warning, TEXT("ThegameCharacter BeginPlay - Checking WeaponSystemClass: %s"), 
        WeaponSystemClass ? TEXT("Valid") : TEXT("NULL"));
        
    // Spawn the weapon system if it doesn't exist yet
    if (!WeaponSystem)
    {
        UE_LOG(LogTemp, Warning, TEXT("Spawning WeaponSystem..."));
        
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        
        // Use the class from the editor if set, otherwise use the base class
        if (WeaponSystemClass)
        {
            WeaponSystem = GetWorld()->SpawnActor<AWeaponSystem>(WeaponSystemClass, 
                                                           GetActorLocation(), 
                                                           GetActorRotation(),
                                                           SpawnParams);
        }
        else
        {
            WeaponSystem = GetWorld()->SpawnActor<AWeaponSystem>(AWeaponSystem::StaticClass(), 
                                                           GetActorLocation(), 
                                                           GetActorRotation(),
                                                           SpawnParams);
        }
                                                          
        if (WeaponSystem)
        {
            UE_LOG(LogTemp, Warning, TEXT("WeaponSystem spawned successfully"));
            
            // First attach the weapon system to the character
            WeaponSystem->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
            
            // Then set the owning actor
            WeaponSystem->SetOwningActor(this);
            
            // Check if we need to restore weapon selection from game instance
            UMyGameInstance* GI = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
if (GI && GI->bShouldApplySavedPlayerStats)
{
    // Apply basic stats if needed (health, speed, etc.)
    CurrentHealth = GI->SavedPlayerHealth;
    MaxHealth = GI->SavedMaxHealth;
    SpeedMultiplier = GI->SavedSpeedMultiplier;
    if (MovementComp)
    {
        MovementComp->MaxWalkSpeed = BaseSpeed * SpeedMultiplier;
    }
    
    // RESTORE BUFF STATES
    bHasRotatingShield = GI->SavedHasRotatingShield;
    bHasReroll = GI->SavedHasReroll;
    SkillDamage = GI->SavedSkillDamage;
    BaseSkillCooldownTime = GI->SavedBaseSkillCooldownTime;
    
    // Update the UI to reflect restored values
    if (InGameUI)
    {
        InGameUI->UpdateValues();
    }
    
    UE_LOG(LogTemp, Warning, TEXT("Restored buff states - HasRotatingShield: %d, HasReroll: %d"), 
        bHasRotatingShield, bHasReroll);
    
    // DEBUG: Log what we're about to restore
    UE_LOG(LogTemp, Warning, TEXT("BEFORE RESTORE: GameInstance has weapons - Slot1=%d, Slot2=%d"), 
        (int32)GI->SavedActiveWeaponSlot1, (int32)GI->SavedActiveWeaponSlot2);
    
    // Wait for a frame to ensure everything is initialized before selecting weapons
    FTimerHandle WeaponSelectionTimerHandle;
GetWorld()->GetTimerManager().SetTimer(
    WeaponSelectionTimerHandle, 
    [this, GI]() 
    {
        if (WeaponSystem && GI)
        {
            // Get the saved weapon values
            int32 Slot1AsInt = GI->SavedWeaponSlot1AsInt;
            int32 Slot2AsInt = GI->SavedWeaponSlot2AsInt;
            
            UE_LOG(LogTemp, Error, TEXT("WEAPON RESTORE: Raw integer values - Slot1=%d, Slot2=%d"), 
                Slot1AsInt, Slot2AsInt);
            
            // Check if we have valid weapon values
            bool bHasValidWeapons = false;
            
            // WeaponA = 0, WeaponB = 1, WeaponC = 2, WeaponD = 3
            // Check if either slot has a valid weapon
            if ((Slot1AsInt >= 0 && Slot1AsInt <= 3) || (Slot2AsInt >= 0 && Slot2AsInt <= 3))
            {
                bHasValidWeapons = true;
            }
            
            // If no valid values from integers, try the enum values
            if (!bHasValidWeapons)
            {
                Slot1AsInt = static_cast<int32>(GI->SavedActiveWeaponSlot1);
                Slot2AsInt = static_cast<int32>(GI->SavedActiveWeaponSlot2);
                
                // Check again with the enum-derived values
                if ((Slot1AsInt >= 0 && Slot1AsInt <= 3) || (Slot2AsInt >= 0 && Slot2AsInt <= 3))
                {
                    bHasValidWeapons = true;
                    UE_LOG(LogTemp, Warning, TEXT("Using enum-derived values: Slot1=%d, Slot2=%d"), 
                        Slot1AsInt, Slot2AsInt);
                }
            }
            
            if (bHasValidWeapons)
            {
                // Convert integers to weapon types
                EWeaponType WeaponType1 = static_cast<EWeaponType>(Slot1AsInt);
                EWeaponType WeaponType2 = static_cast<EWeaponType>(Slot2AsInt);
                
                UE_LOG(LogTemp, Warning, TEXT("APPLYING WEAPONS: Type1=%d, Type2=%d"), 
                    (int32)WeaponType1, (int32)WeaponType2);
                
                // Apply the weapons
                WeaponSystem->SelectWeapons(WeaponType1, WeaponType2);
                
                // Force update with additional delay
                FTimerHandle UpdateTimerHandle;
                GetWorld()->GetTimerManager().SetTimer(
                    UpdateTimerHandle,
                    [this]() {
                        if (WeaponSystem) {
                            WeaponSystem->UpdateWeaponMeshes();
                            WeaponSystem->UpdateWeaponAttachments();
                            WeaponSystem->ApplyWeaponBuffs();
                            
                            UE_LOG(LogTemp, Warning, TEXT("FINAL WEAPON STATE: Slot1=%d, Slot2=%d"), 
                                (int32)WeaponSystem->ActiveWeaponSlot1, 
                                (int32)WeaponSystem->ActiveWeaponSlot2);
                        }
                    },
                    0.2f,
                    false
                );
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("No valid weapons to restore"));
                WeaponSystem->SelectWeapons(EWeaponType::None, EWeaponType::None);
            }
        }
        
        // ACTIVATE ROTATING SHIELD IF PLAYER HAD IT
        if (bHasRotatingShield)
        {
            UE_LOG(LogTemp, Warning, TEXT("Reactivating rotating shield after level transition"));
            ActivateRotatingShield();
        }
    }, 
    0.2f,
    false
                
);
    
    // Reset the flag so we don't apply stats again
    GI->bShouldApplySavedPlayerStats = false;
}
            else
            {
                // No saved stats, default initialization
                FTimerHandle WeaponSelectionTimerHandle;
                GetWorld()->GetTimerManager().SetTimer(
                    WeaponSelectionTimerHandle, 
                    [this]()
                    {
                        if (WeaponSystem)
                        {
                            // Set both weapon slots to None for empty hands
                            WeaponSystem->SelectWeapons(EWeaponType::None, EWeaponType::None);
                
                            // Update weapon meshes (which will hide them)
                            WeaponSystem->UpdateWeaponMeshes();
                
                            UE_LOG(LogTemp, Warning, TEXT("Character initialized with empty hands"));
                        }
                    }, 
                    0.1f, // Small delay to ensure initialization completes
                    false
                );
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to spawn WeaponSystem!"));
        }
    }
	if (!WeaponHitDetector && WeaponSystem)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    
		// Use the class from the editor if set, otherwise use the base class
		if (WeaponHitDetectorClass)
		{
			WeaponHitDetector = GetWorld()->SpawnActor<AWeaponHitDetector>(
				WeaponHitDetectorClass,
				GetActorLocation(),
				GetActorRotation(),
				SpawnParams
			);
		}
		else
		{
			WeaponHitDetector = GetWorld()->SpawnActor<AWeaponHitDetector>(
				AWeaponHitDetector::StaticClass(),
				GetActorLocation(),
				GetActorRotation(),
				SpawnParams
			);
		}
    
		if (WeaponHitDetector)
		{
			UE_LOG(LogTemp, Warning, TEXT("WeaponHitDetector spawned successfully"));
        
			// Attach to character
			WeaponHitDetector->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
        
			// Initialize with references
			WeaponHitDetector->SetOwningCharacter(this);
			WeaponHitDetector->SetWeaponSystem(WeaponSystem);
        
			// Optional: Enable debug drawing during development
			// WeaponHitDetector->bDrawDebug = true;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to spawn WeaponHitDetector!"));
		}
	}
    // Spawn the combo system if it doesn't exist yet
    if (!ComboSkillSystem)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        
        // Use the class from the editor if set, otherwise use the base class
        if (ComboSkillSystemClass)
        {
            ComboSkillSystem = GetWorld()->SpawnActor<AComboSkillSystem>(ComboSkillSystemClass, 
                                                                   GetActorLocation(), 
                                                                   GetActorRotation(),
                                                                   SpawnParams);
        }
        else
        {
            ComboSkillSystem = GetWorld()->SpawnActor<AComboSkillSystem>(AComboSkillSystem::StaticClass(), 
                                                                   GetActorLocation(), 
                                                                   GetActorRotation(),
                                                                   SpawnParams);
        }
                                                                  
        if (ComboSkillSystem)
        {
            // Attach the actor to this character
            ComboSkillSystem->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
            
            // Initialize combo system with a small delay to ensure weapon system is ready
            FTimerHandle ComboSystemTimerHandle;
            GetWorld()->GetTimerManager().SetTimer(
                ComboSystemTimerHandle, 
                [this]()
                {
                    if (ComboSkillSystem && WeaponSystem)
                    {
                        ComboSkillSystem->SetOwningActor(this);
                        ComboSkillSystem->SetWeaponSystem(WeaponSystem);
                        ComboSkillSystem->SetInGameUI(InGameUI);
                    }
                }, 
                0.2f, // Small delay after weapon system
                false
            );
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to spawn ComboSkillSystem!"));
        }
    }
}


// Skill 1
void AThegameCharacter::SkillActivate()
{
	if (!bIsSkillReady)
	{
		UE_LOG(LogTemp, Warning, TEXT("Skill is on cooldown!"));
		return;
	}

	// Rotate character to face mouse cursor before activating the skill
	GetCharacterRotate();

	// Set skill on cooldown
	bIsSkillReady = false;
	SkillCooldownProgress = 0.0f;
	SkillCooldownTime = BaseSkillCooldownTime; // Use the base cooldown time

	// Start the cooldown timer
	GetWorld()->GetTimerManager().SetTimer(SkillCooldownTimerHandle, this, &AThegameCharacter::ResetSkillCooldown, SkillCooldownTime, false);

	// Start the UI update timer
	GetWorld()->GetTimerManager().SetTimer(SkillCooldownUpdateTimerHandle, this, &AThegameCharacter::UpdateSkillCooldownProgress, 0.05f, true);
	
	//Spawn Sword based on the mouse curso
	FHitResult HitResult;
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if(PlayerController && PlayerController->GetHitResultUnderCursor(ECC_Visibility,false, HitResult))
	{
		FVector SpawnLocation = HitResult.Location;
		FRotator SpawnRotation = FRotator(0,0,0);
		SpawnLocation.Z =+ 1750;
    
		// Check if SwordFallDownClass is set
		if (SwordFallDownClass)
		{
			AHolySwordFallDown* Sword = Cast<AHolySwordFallDown>(GetWorld()->SpawnActor(SwordFallDownClass, &SpawnLocation, &SpawnRotation));
			if (Sword)
			{
				Sword->PlaySwordFallDown();
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to spawn HolySwordFallDown even though class was set"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("SwordFallDownClass is not set in the Blueprint"));
		}
    
		SpawnLocation.Z -= 1690;
    
		// Check if TelegraphSystem is set
		if (TelegraphSystem)
		{
			UNiagaraComponent* SpawnEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TelegraphSystem, SpawnLocation, SpawnRotation);
			if (SpawnEffect)
			{
				SpawnEffect->SetFloatParameter(FName("Lifetime"), 1.5f);
				SpawnEffect->SetFloatParameter(FName("Scale"), 500);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to spawn Niagara effect even though TelegraphSystem was set"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("TelegraphSystem is not set in the Blueprint"));
		}
	}
	
	
	// Update UI after using the skill
	if (InGameUI)
	{
		InGameUI->UpdateValues();
	}
}
void AThegameCharacter::UpdateSkillCooldownProgress()
{
	// Increment cooldown progress based on the update interval
	SkillCooldownProgress += (0.05f / SkillCooldownTime); // 0.05 is the timer interval for the update function

	// Clamp the progress to ensure it stays between 0.0 and 1.0
	SkillCooldownProgress = FMath::Clamp(SkillCooldownProgress, 0.0f, 1.0f);

	// Update UI to reflect the current cooldown progress
	if (InGameUI)
	{
		InGameUI->UpdateValues();
	}

	// Stop updating if the cooldown is complete
	if (SkillCooldownProgress >= 1.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(SkillCooldownUpdateTimerHandle);
	}
}
void AThegameCharacter::ResetSkillCooldown()
{
	bIsSkillReady = true;
	// Ensure the progress is set to 1.0 and the UI reflects the change
	SkillCooldownProgress = 1.0f;
	// Stop ONLY this skill's cooldown update timer if it's running
	GetWorld()->GetTimerManager().ClearTimer(SkillCooldownUpdateTimerHandle);
	if (InGameUI)
	{
		InGameUI->UpdateValues();
	}
}

// Skill 2
void AThegameCharacter::Skill2Activate()
{
	if (!bIsSkill2Ready)
	{
		UE_LOG(LogTemp, Warning, TEXT("Skill 2 is on cooldown!"));
		return;
	}

	// Rotate character to face mouse cursor before activating the skill
	GetCharacterRotate();

	// Set skill on cooldown
	bIsSkill2Ready = false;
	Skill2CooldownProgress = 0.0f;
	Skill2CooldownTime = BaseSkill2CooldownTime; // Use the base cooldown time for skill 2

	// Start the cooldown timer - USING SKILL2 FUNCTIONS AND HANDLERS
	GetWorld()->GetTimerManager().SetTimer(Skill2CooldownTimerHandle, this, &AThegameCharacter::ResetSkill2Cooldown, Skill2CooldownTime, false);

	// Start the UI update timer - USING SKILL2 FUNCTIONS AND HANDLERS
	GetWorld()->GetTimerManager().SetTimer(Skill2CooldownUpdateTimerHandle, this, &AThegameCharacter::UpdateSkill2CooldownProgress, 0.05f, true);

	// Execute skill functionality
	FVector Direction = GetActorForwardVector();
	FVector SpawnLocation = GetActorLocation() + Direction * 100;
	FRotator SpawnRotation = Direction.Rotation();

	ALightArrow* SkillArrow = Cast<ALightArrow>(GetWorld()->SpawnActor(LightArrowClass, &SpawnLocation, &SpawnRotation));
	if (SkillArrow)
	{
		// Set the skill damage on the arrow
		SkillArrow->SkillDamage = SkillDamage;
		SkillArrow->MovementComponent->Velocity = Direction * SkillArrow->MovementSpeed;
	}

	// Update UI after using the skill
	if (InGameUI)
	{
		InGameUI->UpdateValues();
	}
}
void AThegameCharacter::UpdateSkill2CooldownProgress()
{
	Skill2CooldownProgress += (0.05f / Skill2CooldownTime);
	Skill2CooldownProgress = FMath::Clamp(Skill2CooldownProgress, 0.0f, 1.0f);

	if (InGameUI)
	{
		InGameUI->UpdateValues();
	}

	if (Skill2CooldownProgress >= 1.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(Skill2CooldownUpdateTimerHandle);
	}
}
void AThegameCharacter::ResetSkill2AnimationFlag()
{
	bIsPlayingSkill2Animation = false;
	UE_LOG(LogTemp, Log, TEXT("Skill2 animation flag reset."));
}
void AThegameCharacter::ResetSkill2Cooldown()
{
	bIsSkill2Ready = true;
	Skill2CooldownProgress = 1.0f;
	// Clear ONLY Skill2's update timer
	GetWorld()->GetTimerManager().ClearTimer(Skill2CooldownUpdateTimerHandle);
	if (InGameUI)
	{
		InGameUI->UpdateValues();
	}
	UE_LOG(LogTemp, Log, TEXT("Skill 2 cooldown reset. Cooldown time: %.2f"), Skill2CooldownTime);
}
void AThegameCharacter::SkillShieldDelay()
{
	// Check if the skill is ready and if the animation is not currently playing
	if (!bIsSkill2Ready || bIsPlayingSkill2Animation)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillShield is on cooldown or already playing."));
		return; // Exit if the skill is not ready
	}

	// Call the actual skill activation function
	Skill2Activate();
}

// SKill Combine
void AThegameCharacter::SkillCombineActivate()
{
	// Rotate character to face mouse cursor before activating the skill
	GetCharacterRotate();
	
	if (ComboSkillSystem)
	{
		ComboSkillSystem->TryActivateCombo();
	}
}
void AThegameCharacter::SelectWeapons(EWeaponType Weapon1, EWeaponType Weapon2)
{
	if (WeaponSystem)
	{
		WeaponSystem->SelectWeapons(Weapon1, Weapon2);
	}
}
// Replace this method in your ThegameCharacter.cpp

float AThegameCharacter::GetSkillCombineCooldownProgress()
{
	if (ComboSkillSystem && WeaponSystem)
	{
		EWeaponType Weapon1 = WeaponSystem->ActiveWeaponSlot1;
		EWeaponType Weapon2 = WeaponSystem->ActiveWeaponSlot2;
        
		// Or use this implementation:
		FComboSkillData ComboData = ComboSkillSystem->GetComboData(Weapon1, Weapon2);
        
		// Calculate and return progress
		if (ComboData.Cooldown <= 0.0f)
		{
			return 1.0f;  // Ready to use (100%)
		}
		
		float Progress = 1.0f - (ComboData.CooldownRemaining / ComboData.Cooldown);
        
		// Add debug logging to track progress values
		UE_LOG(LogTemp, Verbose, TEXT("Combo Cooldown Progress: %.2f (Remaining: %.2f / Total: %.2f)"), 
			   Progress, ComboData.CooldownRemaining, ComboData.Cooldown);
        
		return Progress;
	}
    
	// Fallback to the property if ComboSkillSystem isn't available
	UE_LOG(LogTemp, Verbose, TEXT("Using fallback SkillCombineCooldownProgress: %.2f"), SkillCombineCooldownProgress);
	return SkillCombineCooldownProgress;
}

bool AThegameCharacter::IsSkillCombineReady()
{
	{
		if (ComboSkillSystem && WeaponSystem)
		{
			// Changed from IsComboSkillReady() to IsComboReady()
			return ComboSkillSystem->IsComboReady(
				WeaponSystem->ActiveWeaponSlot1, 
				WeaponSystem->ActiveWeaponSlot2
			);
		}
		return bIsSkillComReady; // Fall back to existing variable
	}
}

void AThegameCharacter::SetPlayerLocation(FVector NewLocation)
{
	FVector ModifiedLocation = NewLocation;

	// Get Default values from charactermovement and sett
	OriginalBrakingDecelerationWalking = GetCharacterMovement()->BrakingDecelerationWalking;
	OriginalGravityScale = GetCharacterMovement()->GravityScale;

	// Disable any movement that comes pushback impulse
	GetCharacterMovement()->BrakingDecelerationWalking = 9999999.0f;
	GetCharacterMovement()->GravityScale = 0;

	//Zero out the z velocity component
	ModifiedLocation.Z = GetActorLocation().Z;   //  Important: Keep the existing Z coordinate
	GetCapsuleComponent()->SetWorldLocation(ModifiedLocation, false, nullptr, ETeleportType::TeleportPhysics);

	// Reset the speed.
	GetCharacterMovement()->BrakingDecelerationWalking = OriginalBrakingDecelerationWalking;

	//Zero out the z to remove the physics velocity
	GetCharacterMovement()->Velocity.Z = 0;

	//Get back old values on x time
	FTimerHandle MemberTimerHandle;
	GetWorldTimerManager().SetTimer(MemberTimerHandle, this, &AThegameCharacter::ReApplyGravity, 0.2f, false);

}

void AThegameCharacter::ReApplyGravity() 
{
	//Just reapply gravity and velocity
	GetCharacterMovement()->BrakingDecelerationWalking = OriginalBrakingDecelerationWalking;
	GetCharacterMovement()->GravityScale = OriginalGravityScale;
	GetCharacterMovement()->Velocity.Z = 0;
}

void AThegameCharacter::SetFixedCameraLocation(FVector NewLocation, float DeltaTime)
{
	// Get the current camera location
	FVector CurrentLocation = FollowCamera->GetComponentLocation();

	// Use FMath::VInterpTo to smoothly interpolate between the current and new locations
	FVector InterpLocation = FMath::VInterpTo(CurrentLocation, NewLocation, DeltaTime, 10.0f); 
    
	// set the Z of the camera that is on top of his head
	InterpLocation.Z = CurrentLocation.Z;

	// Set the new camera location
	FollowCamera->SetWorldLocation(InterpLocation);
}

void AThegameCharacter::TestWeaponAttachments()
{
	if (WeaponSystem)
	{
		// Test with sword in right hand, shield in left hand
		WeaponSystem->SelectWeapons(EWeaponType::WeaponA, EWeaponType::WeaponB);
        
		// Draw debug spheres at hand locations
		USkeletalMeshComponent* SkelMesh = GetMesh();
		if (SkelMesh)
		{
			FVector LeftHandLoc = SkelMesh->GetBoneLocation(FName("hand_l"));
			FVector RightHandLoc = SkelMesh->GetBoneLocation(FName("hand_r"));
            
			DrawDebugSphere(GetWorld(), LeftHandLoc, 5.0f, 12, FColor::Blue, false, 5.0f);
			DrawDebugSphere(GetWorld(), RightHandLoc, 5.0f, 12, FColor::Red, false, 5.0f);
		}
        
		UE_LOG(LogTemp, Warning, TEXT("Weapon test - Set Sword (A) and Shield (B)"));
	}
}

void AThegameCharacter::StartAttackHitDetection()
{
	UE_LOG(LogTemp, Warning, TEXT("StartAttackHitDetection called"));
	if (WeaponHitDetector)
	{
		UE_LOG(LogTemp, Warning, TEXT("Calling WeaponHitDetector->StartSwing()"));
		WeaponHitDetector->StartSwing();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponHitDetector is null!"));
	}
}

void AThegameCharacter::EndAttackHitDetection()
{
	UE_LOG(LogTemp, Warning, TEXT("EndAttackHitDetection called"));
	if (WeaponHitDetector)
	{
		WeaponHitDetector->EndSwing();
	}
}

void AThegameCharacter::TestWeaponHitDetection()
{
	UE_LOG(LogTemp, Warning, TEXT("TestWeaponHitDetection called manually"));
    
	// Make sure we have a weapon equipped
	if (WeaponSystem && WeaponSystem->ActiveWeaponSlot1 == EWeaponType::None && WeaponSystem->ActiveWeaponSlot2 == EWeaponType::None)
	{
		UE_LOG(LogTemp, Warning, TEXT("No weapon equipped - equipping sword for test"));
		WeaponSystem->SelectWeapons(EWeaponType::WeaponA, EWeaponType::WeaponB);
	}
    
	// Enable debug drawing
	if (WeaponHitDetector)
	{
		UE_LOG(LogTemp, Warning, TEXT("Enabling debug drawing for test"));
		WeaponHitDetector->bDrawDebug = true;
        
		UE_LOG(LogTemp, Warning, TEXT("Starting test weapon swing"));
		WeaponHitDetector->StartSwing();
        
		// End the swing after a brief delay
		FTimerHandle EndSwingTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
			EndSwingTimerHandle,
			[this]()
			{
				if (WeaponHitDetector)
				{
					UE_LOG(LogTemp, Warning, TEXT("Ending test weapon swing"));
					WeaponHitDetector->EndSwing();
				}
			},
			0.5f, // Half-second duration
			false
		);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponHitDetector is null!"));
	}
}

void AThegameCharacter::ActivateRotatingShield()
{
	// Check if shield actor already exists (not just the flag)
	if (ActiveRotatingShield && IsValid(ActiveRotatingShield))
	{
		UE_LOG(LogTemp, Warning, TEXT("Rotating shield actor is already active!"));
		return;
	}
    
	// Check if we have a shield class to spawn
	if (!RotatingShieldClass)
	{
		UE_LOG(LogTemp, Error, TEXT("RotatingShieldClass is not set in the Blueprint!"));
		return;
	}
    
	// If we had the shield before but actor is missing (like after level transition), clear the reference
	if (ActiveRotatingShield && !IsValid(ActiveRotatingShield))
	{
		UE_LOG(LogTemp, Warning, TEXT("Clearing invalid rotating shield reference"));
		ActiveRotatingShield = nullptr;
	}
    
	// Spawn the rotating shield
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    
	FVector SpawnLocation = GetActorLocation();
	FRotator SpawnRotation = FRotator::ZeroRotator;
    
	ActiveRotatingShield = GetWorld()->SpawnActor<ARotatingShield>(
		RotatingShieldClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);
    
	if (ActiveRotatingShield)
	{
		ActiveRotatingShield->InitializeShield(this);
		bHasRotatingShield = true;
        
		// Additional debug to verify the shield knows its owner
		UE_LOG(LogTemp, Warning, TEXT("Rotating shield activated for character: %s"), *GetName());
        
		// Force an immediate position update
		ActiveRotatingShield->UpdateShieldPosition(0.0f);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn rotating shield!"));
	}
}

void AThegameCharacter::DeactivateRotatingShield()
{
	if (ActiveRotatingShield)
	{
		ActiveRotatingShield->Destroy();
		ActiveRotatingShield = nullptr;
		bHasRotatingShield = false;
		
		UE_LOG(LogTemp, Warning, TEXT("Rotating shield deactivated"));
	}
}

// Setup-Rewind
void AThegameCharacter::Setup()
{
	// Check if not dashing before allowing setup
	if (bAlreadySetup == false && !bIsDashing)
	{
		FVector Location = GetActorLocation() - FVector(0, 0, 100);
		SpawnTeleport = (GetWorld()->SpawnActor(LandmarkClass, &Location));
		bAlreadySetup = true;
		bCanRewind = true;

		// Update the UI to show "R" (for Rewind)
		if (InGameUI)
		{
			InGameUI->SetSetupRewindText("R");
		}
	}
}
void AThegameCharacter::Rewind()
{
	if (bCanRewind)
	{
		FVector TeleportLocation = SpawnTeleport->GetActorLocation();
        
		// Add a height offset to avoid clipping into slopes
		TeleportLocation.Z += 50.0f; // Adjust this value as needed
        
		SetActorLocation(TeleportLocation);
		SpawnTeleport->Destroy();
		bCanRewind = false;
		bAlreadySetup = false;

		// Reset the UI to show "E" (for Setup)
		if (InGameUI)
		{
			InGameUI->SetSetupRewindText("E");
		}

		bIsSkill2Ready = true;
		bIsSkillReady = true;
		bIsSkillComReady = true;
	}
}

// Dash
void AThegameCharacter::Dashing()
{
	if(CanDashing)
	{
		CanDashing = false;
		bIsDashing = true; // Set dashing state to true
        
		const FVector ForwardDirection = this->GetActorRotation().Vector();
		LaunchCharacter(ForwardDirection * DashDistance, true, true);
		PlayAnimMontage(DashAnimation);
		GetWorld()->GetTimerManager().SetTimer(DashReset, this, &AThegameCharacter::DashingReset, 1.0f, false);
	}
}
void AThegameCharacter::DashingReset()
{
	CanDashing = true;
	bIsDashing = false;
}

void AThegameCharacter::SetInGameUIVisibility(bool bVisible)
{
	if (InGameUI)
	{
		if (bVisible && !InGameUI->IsInViewport())
		{
			InGameUI->AddToViewport();
			InGameUI->UpdateValues();
			UE_LOG(LogTemp, Log, TEXT("Player UI added to viewport"));
		}
		else if (!bVisible && InGameUI->IsInViewport())
		{
			InGameUI->RemoveFromParent();
			UE_LOG(LogTemp, Log, TEXT("Player UI removed from viewport"));
		}
	}
}

void AThegameCharacter::SpawnBushWall()
{
	if(bCanRiseWall)
	{
		bCanRiseWall = false;
		FVector SpawnLocation = GetActorForwardVector() + FVector(100,0,0);
		FRotator SpawnRotation = GetActorRotation();
		ABushWallSkill* BushWall = Cast<ABushWallSkill>(GetWorld()->SpawnActor(BushWallClass, &SpawnLocation, &SpawnRotation));
		BushWall->SpawnBush();
	}
	GetWorldTimerManager().SetTimer(BushCoolDownTime, this, &AThegameCharacter::SkillBushAvailable, 5.0f, false );
}

void AThegameCharacter::SkillBushAvailable()
{
	bCanRiseWall = true;
}

//Setting Game
void AThegameCharacter::HandlePause()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		if (!bIsPaused)
		{
			// Show pause menu
			if (PauseMenuWidgetClass)
			{
				UUserWidget* PauseMenuWidget = CreateWidget<UUserWidget>(PlayerController, PauseMenuWidgetClass);
				if (PauseMenuWidget)
				{
					PauseMenuWidget->AddToViewport();
					PlayerController->SetPause(true);
					PlayerController->SetInputMode(FInputModeGameAndUI());
					PlayerController->bShowMouseCursor = true;
				}
			}
		}
		else
		{
			// Hide pause menu
			PlayerController->SetPause(false);
			PlayerController->SetInputMode(FInputModeGameAndUI());
			PlayerController->bShowMouseCursor = false;
		}
		bIsPaused = !bIsPaused;
	}
}
void AThegameCharacter::ResumeGame()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		// Unpause the game and restore input for gameplay
		PlayerController->SetPause(false);
		PlayerController->SetInputMode(FInputModeGameAndUI());  // Return to game-only input
		PlayerController->bShowMouseCursor = true;  // Hide mouse cursor for gameplay

		bIsPaused = false;  // Ensure pause state is reset
	}
}

//Tick
void AThegameCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	// Apply speed multiplier every frame
	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	if (MovementComp)
	{
		// Apply the speed multiplier to the base speed
		MovementComp->MaxWalkSpeed = BaseSpeed * SpeedMultiplier;
	}
}

//////////////////////////////////////////////////////////////////////////
// Input
void AThegameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AThegameCharacter::Move);

		//Use Weapon Skill
		EnhancedInputComponent->BindAction(WeaponSkillAction, ETriggerEvent::Started, this, &AThegameCharacter::SkillActivate);

		//Use 2 Weapon Skill		
		EnhancedInputComponent->BindAction(Weapon2SkillAction, ETriggerEvent::Started, this, &AThegameCharacter::SkillShieldDelay);

		//Use Combine Weapon Skill
		EnhancedInputComponent->BindAction(WeaponCombineSkillAction, ETriggerEvent::Started, this, &AThegameCharacter::SkillCombineActivate);

		//Dashing
		//EnhancedInputComponent->BindAction(DashingAction, ETriggerEvent::Started, this, &AThegameCharacter::Dashing);

		//Setup
		EnhancedInputComponent->BindAction(SetupAction, ETriggerEvent::Started, this, &AThegameCharacter::Setup);

		//Rewind
		EnhancedInputComponent->BindAction(RewindAction, ETriggerEvent::Started, this, &AThegameCharacter::Rewind);

		EnhancedInputComponent->BindAction(PrimaryAttackAction, ETriggerEvent::Started, this, &AThegameCharacter::HandlePrimaryAttack);

		//Pause
		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Started, this, &AThegameCharacter::HandlePause);
		
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}
void AThegameCharacter::GetCharacterRotate()
{
	APlayerController* PController = GetWorld()->GetFirstPlayerController();
	if (!PController)
		return;

	float xMouse, yMouse;
	PController->GetMousePosition(xMouse, yMouse);

	FVector CharLoc = GetActorLocation();

	FVector2D CharInScreen;
	PController->ProjectWorldLocationToScreen(CharLoc, CharInScreen);

	FVector2D Result;
	Result.X = -(yMouse - CharInScreen.Y);
	Result.Y = xMouse - CharInScreen.X;

	// Make sure we don't try to normalize zero vector
	if (Result.SizeSquared() < KINDA_SMALL_NUMBER)
		return;

	// Get angle rotation and rotation Character
	float angle = FMath::RadiansToDegrees(FMath::Acos(Result.X / Result.Size()));

	if (Result.Y < 0)
		angle = 360 - angle;

	Mouse =  FRotator(0, angle, 0);

	SetActorRotation(Mouse);
}
void AThegameCharacter::GetDamage(float InDamageAmount)
{
	CurrentHealth -= InDamageAmount;
    InGameUI->UpdateValues();
    if (CurrentHealth <= 0.0f)
    {
        UE_LOG(LogTemp, Warning, TEXT("Player has died."));
        CurrentHealth = 0.0f;

        // Get the game instance
        UMyGameInstance* GameInstance = Cast<UMyGameInstance>(GetGameInstance());
        if (GameInstance)
        {
            // Delete all save games
            GameInstance->DeleteAllSaveGames();
            
            // Reset all saved data in game instance
            GameInstance->SavedPlayerHealth = 100.0f;
            GameInstance->SavedMaxHealth = 100.0f;
            GameInstance->SavedSpeedMultiplier = 1.0f;
            GameInstance->SavedBaseSpeed = 700.0f;
            GameInstance->SavedActiveWeaponSlot1 = EWeaponType::None;
            GameInstance->SavedActiveWeaponSlot2 = EWeaponType::None;
            GameInstance->SavedWeaponSlot1AsInt = 0;
            GameInstance->SavedWeaponSlot2AsInt = 0;
            GameInstance->bShouldApplySavedTransform = false;
            GameInstance->bShouldApplySavedPlayerStats = false;
            
            // Reset player stats to defaults
            CurrentHealth = BaseMaxHealth;
            MaxHealth = BaseMaxHealth;
            SpeedMultiplier = 1.0f;
            BaseSpeed = 700.0f;
            DamageAmount = BaseDamageAmount;
            
            // Reset skill stats
            BaseSkillCooldownTime = 3.0f; // Default value
            SkillDamage = 10.0f; // Default value
            
            // Reset buff flags
            bHasReroll = false;
            bHasRotatingShield = false;
            
            // Deactivate rotating shield if active
            if (ActiveRotatingShield)
            {
                DeactivateRotatingShield();
            }
            
            // Reset weapons to None
            if (WeaponSystem)
            {
                WeaponSystem->SelectWeapons(EWeaponType::None, EWeaponType::None);
            }
            
            // Disable player input temporarily
            APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
            if (PlayerController)
            {
                PlayerController->SetIgnoreMoveInput(true);
                PlayerController->SetIgnoreLookInput(true);
                
                // Hide mouse cursor
                PlayerController->bShowMouseCursor = false;
                PlayerController->SetInputMode(FInputModeGameOnly());
            }
            
            // Delay for death effect (optional - you can remove this if you want instant respawn)
            FTimerHandle DeathDelayTimer;
            GetWorld()->GetTimerManager().SetTimer(DeathDelayTimer, [this, GameInstance]()
            {
                // Load woodlands_Start level
                UGameplayStatics::OpenLevel(this, TEXT("woodlands_Start"));
            }, 0.5f, false); // 0.5 second delay before respawn
        }
    }
}
void AThegameCharacter::ResetToDefaultStats()
{
	// Reset health
	CurrentHealth = BaseMaxHealth;
	MaxHealth = BaseMaxHealth;
    
	// Reset movement
	SpeedMultiplier = 1.0f;
	BaseSpeed = 700.0f;
    
	// Reset damage
	DamageAmount = BaseDamageAmount;
    
	// Reset skills
	BaseSkillCooldownTime = 3.0f;
	SkillDamage = 10.0f;
	BaseSkill2CooldownTime = 2.0f;
    
	// Reset buff flags
	bHasReroll = false;
	bHasRotatingShield = false;
    
	// Reset skill cooldowns
	bIsSkillReady = true;
	bIsSkill2Ready = true;
	bIsSkillComReady = true;
	SkillCooldownProgress = 1.0f;
	Skill2CooldownProgress = 1.0f;
	SkillCombineCooldownProgress = 1.0f;
    
	// Clear any active timers
	GetWorld()->GetTimerManager().ClearTimer(SkillCooldownHandle);
	GetWorld()->GetTimerManager().ClearTimer(Skill2CooldownHandle);
	GetWorld()->GetTimerManager().ClearTimer(CombineCooldownHandle);
    
	// Deactivate rotating shield if active
	if (ActiveRotatingShield)
	{
		DeactivateRotatingShield();
	}
    
	// Reset weapons
	if (WeaponSystem)
	{
		WeaponSystem->SelectWeapons(EWeaponType::None, EWeaponType::None);
	}
    
	// Update UI
	if (InGameUI)
	{
		InGameUI->UpdateValues();
	}
    
	UE_LOG(LogTemp, Log, TEXT("Player stats reset to defaults"));
}

void AThegameCharacter::GetDamageDot(float Damage)
{
	UGameplayStatics::ApplyDamage(this, Damage, nullptr, this, nullptr);
}

void AThegameCharacter::HealthPlayer(float amount)
{
	CurrentHealth += amount;
}

void AThegameCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		
			// find out which way is forward
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get forward vector
			const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
			// get right vector 
			const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

			// add movement 
			AddMovementInput(ForwardDirection, MovementVector.Y);
			AddMovementInput(RightDirection, MovementVector.X);
		
	}
	
}


// Override or add to your existing attack input handling
void AThegameCharacter::HandlePrimaryAttack()
{
	// Check which weapon is equipped in the attack slot
	if (!WeaponSystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("No WeaponSystem found!"));
		return;
	}
    
	// Determine which attack weapon is equipped
	EWeaponType AttackWeapon = EWeaponType::None;
    
	// Check slot 1 first (primary attack slot)
	if (WeaponSystem->IsAttackWeapon(WeaponSystem->ActiveWeaponSlot1))
	{
		AttackWeapon = WeaponSystem->ActiveWeaponSlot1;
	}
	// If no attack weapon in slot 1, check slot 2
	else if (WeaponSystem->IsAttackWeapon(WeaponSystem->ActiveWeaponSlot2))
	{
		AttackWeapon = WeaponSystem->ActiveWeaponSlot2;
	}
    
	// Perform appropriate attack based on weapon type
	switch (AttackWeapon)
	{
	case EWeaponType::WeaponA: // Sword - melee attack
		// Call the blueprint event for sword attack
			OnSwordAttack();
		break;
            
	case EWeaponType::WeaponC: // Crossbow - ranged attack
		// Fire crossbow arrow
			FireCrossbowArrow();
		break;
            
	case EWeaponType::None:
	default:
		UE_LOG(LogTemp, Warning, TEXT("No attack weapon equipped!"));
		break;
	}
}
void AThegameCharacter::FireCrossbowArrow()
{
    // Check fire rate cooldown
    if (!bCanFireCrossbow)
    {
        return;
    }
    
    // Check if we have a crossbow arrow class set
    if (!CrossbowArrowClass)
    {
        UE_LOG(LogTemp, Error, TEXT("CrossbowArrowClass is not set in the Blueprint!"));
        return;
    }
    
    // Rotate character to face mouse cursor
    GetCharacterRotate();
    
    // Get player controller and mouse position
    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    if (!PlayerController)
    {
        return;
    }
    
    // Get viewport size to check if mouse is on screen
    int32 ViewportSizeX, ViewportSizeY;
    PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);
    
    // Get mouse position on screen
    float MouseX, MouseY;
    PlayerController->GetMousePosition(MouseX, MouseY);
    
    // Calculate shoot direction
    FVector ShootDirection;
    
    // Method 1: Try to get direction from camera to world
    FVector WorldLocation, WorldDirection;
    if (PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
    {
        // Trace from camera to find what we're aiming at
        FVector CameraLocation = PlayerController->PlayerCameraManager->GetCameraLocation();
        FVector TraceEnd = CameraLocation + (WorldDirection * 10000.0f);
        
        FHitResult HitResult;
        FCollisionQueryParams QueryParams;
        QueryParams.AddIgnoredActor(this);
        QueryParams.bTraceComplex = false;
        
        bool bHit = GetWorld()->LineTraceSingleByChannel(
            HitResult,
            CameraLocation,
            TraceEnd,
            ECC_Visibility,
            QueryParams
        );
        
        // Get the target point
        FVector TargetPoint;
        if (bHit)
        {
            TargetPoint = HitResult.Location;
        }
        else
        {
            // If no hit, project a point far away in the trace direction
            TargetPoint = CameraLocation + (WorldDirection * 5000.0f);
        }
        
        // Calculate direction from character center to target point
        FVector CharacterCenter = GetActorLocation() + FVector(0, 0, 50.0f); // Chest height
        ShootDirection = (TargetPoint - CharacterCenter).GetSafeNormal();
        
        // If the target is too close or invalid, use a different approach
        float DistanceToTarget = FVector::Dist(TargetPoint, CharacterCenter);
        if (DistanceToTarget < 200.0f || ShootDirection.IsNearlyZero())
        {
            // Convert mouse position to direction relative to screen center
            float NormalizedMouseX = (MouseX / ViewportSizeX - 0.5f) * 2.0f;
            float NormalizedMouseY = (MouseY / ViewportSizeY - 0.5f) * 2.0f;
            
            // Get camera forward and right vectors
            FVector CameraForward = PlayerController->PlayerCameraManager->GetCameraRotation().Vector();
            FVector CameraRight = PlayerController->PlayerCameraManager->GetActorRightVector();
            FVector CameraUp = PlayerController->PlayerCameraManager->GetActorUpVector();
            
            // Create direction based on mouse offset from center
            CameraForward.Z = 0;
            CameraForward.Normalize();
            ShootDirection = CameraForward + (CameraRight * NormalizedMouseX * 0.5f) - (CameraUp * NormalizedMouseY * 0.3f);
            ShootDirection.Normalize();
        }
    }
    else
    {
        // Fallback: use character forward direction
        ShootDirection = GetActorForwardVector();
    }
    
    // Final safety check
    if (ShootDirection.IsNearlyZero())
    {
        ShootDirection = GetActorForwardVector();
    }
    
    // Get spawn location - further in front and higher up to avoid collision
    FVector SpawnLocation = GetActorLocation() + 
                           (GetActorForwardVector() * 150.0f) + // Further forward
                           FVector(0, 0, 80.0f); // Higher up at shoulder level
    
    // Spawn the arrow
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = GetInstigator();
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    
    ACrossbowArrow* Arrow = GetWorld()->SpawnActor<ACrossbowArrow>(
        CrossbowArrowClass,
        SpawnLocation,
        ShootDirection.Rotation(),
        SpawnParams
    );
    
    if (Arrow)
    {
        Arrow->SetOwnerCharacter(this);
        Arrow->FireInDirection(ShootDirection);
        
        // Set cooldown
        bCanFireCrossbow = false;
        GetWorld()->GetTimerManager().SetTimer(
            CrossbowCooldownTimer,
            this,
            &AThegameCharacter::ResetCrossbowCooldown,
            CrossbowFireRate,
            false
        );
        
        UE_LOG(LogTemp, Warning, TEXT("Fired crossbow arrow - Direction: %s"), *ShootDirection.ToString());
    }
}
void AThegameCharacter::ResetCrossbowCooldown()
{
	bCanFireCrossbow = true;
}