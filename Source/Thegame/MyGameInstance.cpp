// MyGameInstance.cpp

#include "MyGameInstance.h"

#include "GlobalSettingsSaveGame.h"
#include "InGameSaveGame.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "InGameSetting.h"
#include "SaveGameFunctions.h"
#include "ThegameCharacter.h"

// Initialize static members
const FString UMyGameInstance::SaveSlot1 = TEXT("SaveSlot_1");
const FString UMyGameInstance::SaveSlot2 = TEXT("SaveSlot_2");
const FString UMyGameInstance::SaveSlot3 = TEXT("SaveSlot_3");
int32 UMyGameInstance::CurrentAutoSaveSlot = 1;
void UMyGameInstance::Init()
{
    Super::Init();
    LoadGlobalSettings();
    // Initialize previous map names as empty
    PreviousMapName = TEXT("");
    LastAutoSavedMapName = TEXT("");
    bIsLoadingSaveGame = false;
    
    // Create and add the in-game settings widget
    if (InGameSettingClass)
    {
        UInGameSetting* InGameSettingWidget = CreateWidget<UInGameSetting>(this, InGameSettingClass);
        if (InGameSettingWidget)
        {
            InGameSettingWidget->AddToViewport();
            UE_LOG(LogTemp, Log, TEXT("InGameSetting widget created and added to viewport."));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create InGameSetting widget."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("InGameSettingClass is not set in the GameInstance."));
    }
    
    // Register for the PostLoadMapWithWorld delegate
    FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UMyGameInstance::OnPostLoadMap);
}

void UMyGameInstance::OnPostLoadMap(UWorld* LoadedWorld)
{
    // Apply global exposure to the new map
    ApplyGlobalExposure(LoadedWorld);
    
    // Get the name of the loaded map
    FString LoadedMapName = UGameplayStatics::GetCurrentLevelName(LoadedWorld);
    UE_LOG(LogTemp, Log, TEXT("New map loaded: %s"), *LoadedMapName);
    
    // IMPORTANT: Always ensure player input is re-enabled after level loads
    APlayerController* PC = UGameplayStatics::GetPlayerController(LoadedWorld, 0);
    if (PC)
    {
        PC->SetIgnoreMoveInput(false);
        PC->SetIgnoreLookInput(false);
        UE_LOG(LogTemp, Log, TEXT("Re-enabling player input after level load"));
        
        // Check if this is a death respawn to woodlands_Start
        bool bIsDeathRespawn = false;
        if (LoadedMapName.Equals(TEXT("woodlands_Start")))
        {
            // Check if player had died (CurrentHealth would be 0 or saved health would be 0)
            if (SavedPlayerHealth <= 0.0f)
            {
                bIsDeathRespawn = true;
                UE_LOG(LogTemp, Log, TEXT("Death respawn detected - resetting player stats"));
            }
        }
        
        // Apply saved player stats if needed (but not on death respawn)
        if (bShouldApplySavedPlayerStats && !bIsDeathRespawn)
        {
            APawn* Pawn = PC->GetPawn();
            if (Pawn)
            {
                AThegameCharacter* PlayerCharacter = Cast<AThegameCharacter>(Pawn);
                if (PlayerCharacter)
                {
                    // Apply saved stats
                    PlayerCharacter->CurrentHealth = (SavedPlayerHealth > 0) ? SavedPlayerHealth : PlayerCharacter->MaxHealth;
                    PlayerCharacter->MaxHealth = (SavedMaxHealth > 0) ? SavedMaxHealth : 100.0f;
                    PlayerCharacter->SpeedMultiplier = (SavedSpeedMultiplier > 0) ? SavedSpeedMultiplier : 1.0f;
                    PlayerCharacter->BaseSpeed = (SavedBaseSpeed > 0) ? SavedBaseSpeed : 700.0f;
                
                    // Apply saved weapons
                    if (PlayerCharacter->WeaponSystem)
                    {
                        EWeaponType Weapon1 = SavedActiveWeaponSlot1;
                        EWeaponType Weapon2 = SavedActiveWeaponSlot2;
                    
                        // Use integer values as fallback
                        if (Weapon1 == EWeaponType::None && SavedWeaponSlot1AsInt > 0)
                        {
                            Weapon1 = (EWeaponType)SavedWeaponSlot1AsInt;
                        }
                        if (Weapon2 == EWeaponType::None && SavedWeaponSlot2AsInt > 0)
                        {
                            Weapon2 = (EWeaponType)SavedWeaponSlot2AsInt;
                        }
                    
                        UE_LOG(LogTemp, Warning, TEXT("OnPostLoadMap: Applying weapons - Slot1=%d, Slot2=%d"), 
                            (int32)Weapon1, (int32)Weapon2);
                    
                        // Delay weapon application to ensure everything is initialized
                        FTimerHandle WeaponTimer;
                        LoadedWorld->GetTimerManager().SetTimer(WeaponTimer, [PlayerCharacter, Weapon1, Weapon2]()
                        {
                            if (PlayerCharacter && PlayerCharacter->WeaponSystem)
                            {
                                PlayerCharacter->WeaponSystem->SelectWeapons(Weapon1, Weapon2);
                            }
                        }, 0.5f, false);
                    }
                
                    // Update UI
                    if (PlayerCharacter->InGameUI)
                    {
                        PlayerCharacter->InGameUI->UpdateValues();
                    }
                
                    UE_LOG(LogTemp, Log, TEXT("Applied saved player stats and weapons"));
                }
            }
        }
        // Handle death respawn
        else if (bIsDeathRespawn)
        {
            APawn* Pawn = PC->GetPawn();
            if (Pawn)
            {
                AThegameCharacter* PlayerCharacter = Cast<AThegameCharacter>(Pawn);
                if (PlayerCharacter)
                {
                    // Reset all stats to defaults
                    PlayerCharacter->ResetToDefaultStats();
                    
                    // Force auto-save after death respawn
                    FTimerHandle AutoSaveTimerHandle;
                    LoadedWorld->GetTimerManager().SetTimer(
                        AutoSaveTimerHandle,
                        [this, LoadedMapName]()
                        {
                            UE_LOG(LogTemp, Log, TEXT("Death respawn auto-save triggered for %s"), *LoadedMapName);
                            AutoSaveGame();
                            
                            // Update the saved map names
                            PreviousMapName = LoadedMapName;
                            LastAutoSavedMapName = LoadedMapName;
                        },
                        1.0f, // 1 second delay
                        false
                    );
                }
            }
        }
        
        // Also apply saved transform if needed (but not on death respawn)
        if (bShouldApplySavedTransform && !bIsDeathRespawn)
        {
            APawn* Pawn = PC->GetPawn();
            if (Pawn)
            {
                Pawn->SetActorLocation(SavedPlayerTransform.GetLocation());
                Pawn->SetActorRotation(SavedPlayerTransform.GetRotation());
                bShouldApplySavedTransform = false;
                UE_LOG(LogTemp, Log, TEXT("Applied saved transform to player."));
            }
        }
    }
    
    if (LoadedMapName.Equals(TEXT("Level")) || 
        LoadedMapName.Contains(TEXT("Menu")) || 
        LoadedMapName.Contains(TEXT("Title")))
    {
        UE_LOG(LogTemp, Log, TEXT("Menu level detected; auto-save skipped for map: %s"), *LoadedMapName);
        // Update previous map name but don't auto-save
        PreviousMapName = LoadedMapName;
        return;
    }
    
    // Check if this load came from a saved game
    if (bIsLoadingSaveGame)
    {
        UE_LOG(LogTemp, Log, TEXT("LoadGame detected; auto-save skipped for map: %s"), *LoadedMapName);
        // Reset the flag so that subsequent non-load operations auto-save as usual
        bIsLoadingSaveGame = false;
        // Update map names to avoid triggering auto-save next time if the map didn't change
        PreviousMapName = LoadedMapName;
        LastAutoSavedMapName = LoadedMapName;
        return;
    }

    // Determine if we should auto-save
    bool bShouldSave = false;
    if (PreviousMapName.IsEmpty() || !LoadedMapName.Equals(PreviousMapName))
    {
        bShouldSave = true;
    }

    if (LoadedMapName.Equals(TEXT("woodlands_Start")))
    {
        bShouldSave = bShouldSave || !LoadedMapName.Equals(LastAutoSavedMapName);
    }

    if (bShouldSave)
    {
        // CRITICAL FIX: Delay auto-save to ensure weapons are restored first
        FTimerHandle AutoSaveTimerHandle;
        LoadedWorld->GetTimerManager().SetTimer(
            AutoSaveTimerHandle,
            [this, LoadedMapName]()
            {
                UE_LOG(LogTemp, Log, TEXT("Delayed auto-save triggered for %s"), *LoadedMapName);
                AutoSaveGame();
                
                // Update the saved map names
                PreviousMapName = LoadedMapName;
                LastAutoSavedMapName = LoadedMapName;
            },
            1.0f, // 1 second delay to ensure weapons are loaded
            false
        );
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("No auto-save triggered for %s because it's the same as the last saved map."), *LoadedMapName);
    }

    // Recreate the settings widget
    if (PC && InGameSettingClass)
    {
        UInGameSetting* InGameSettingWidget = CreateWidget<UInGameSetting>(PC, InGameSettingClass);
        if (InGameSettingWidget)
        {
            InGameSettingWidget->AddToViewport();
            UE_LOG(LogTemp, Log, TEXT("Recreated InGameSetting widget on new map."));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to recreate InGameSetting widget on new map."));
        }
    }
}

void UMyGameInstance::AutoSaveGame()
{
    // Create a save game object
    UInGameSaveGame* SaveGameInstance = Cast<UInGameSaveGame>(UGameplayStatics::CreateSaveGameObject(UInGameSaveGame::StaticClass()));
    
    // Save the current map name
    FString CurrentMapName = UGameplayStatics::GetCurrentLevelName(GetWorld());
    SaveGameInstance->CurrentMap = CurrentMapName;
    SaveGameInstance->Timestamp = FDateTime::Now().ToString();
    UE_LOG(LogTemp, Log, TEXT("Auto-saving game on Map: %s"), *CurrentMapName);

    // Save player location and rotation
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC)
    {
        APawn* Pawn = PC->GetPawn();
        if (Pawn)
        {
            SaveGameInstance->PlayerLocation = Pawn->GetActorLocation();
            SaveGameInstance->PlayerRotation = Pawn->GetActorRotation();
            
            // Save player character state
            AThegameCharacter* PlayerCharacter = Cast<AThegameCharacter>(Pawn);
            if (PlayerCharacter)
            {
                // Save health
                SaveGameInstance->CurrentHealth = PlayerCharacter->CurrentHealth;
                SaveGameInstance->MaxHealth = PlayerCharacter->MaxHealth;
                
                // Save movement
                SaveGameInstance->SpeedMultiplier = PlayerCharacter->SpeedMultiplier;
                SaveGameInstance->BaseSpeed = PlayerCharacter->BaseSpeed;
                
                // SAVE BUFF STATES TO SAVE GAME
                SaveGameInstance->bHasRotatingShield = PlayerCharacter->bHasRotatingShield;
                SaveGameInstance->bHasReroll = PlayerCharacter->bHasReroll;
                SaveGameInstance->SkillDamage = PlayerCharacter->SkillDamage;
                SaveGameInstance->BaseSkillCooldownTime = PlayerCharacter->BaseSkillCooldownTime;
                
                // Also update the game instance variables
                SavedHasRotatingShield = PlayerCharacter->bHasRotatingShield;
                SavedHasReroll = PlayerCharacter->bHasReroll;
                SavedSkillDamage = PlayerCharacter->SkillDamage;
                SavedBaseSkillCooldownTime = PlayerCharacter->BaseSkillCooldownTime;
                
                // NEW: Save equipped weapons
                if (PlayerCharacter->WeaponSystem)
                {
                    // Log the current weapon state before saving
                    UE_LOG(LogTemp, Warning, TEXT("AUTO-SAVE: Current character weapons - Slot1=%d, Slot2=%d"), 
                        (int32)PlayerCharacter->WeaponSystem->ActiveWeaponSlot1, 
                        (int32)PlayerCharacter->WeaponSystem->ActiveWeaponSlot2);
                    
                    // Save the weapon slots both to the game instance and save game object
                    SavedActiveWeaponSlot1 = PlayerCharacter->WeaponSystem->ActiveWeaponSlot1;
                    SavedActiveWeaponSlot2 = PlayerCharacter->WeaponSystem->ActiveWeaponSlot2;
                    
                    // Save to the save game instance
                    SaveGameInstance->Weapon1 = (int32)SavedActiveWeaponSlot1;
                    SaveGameInstance->Weapon2 = (int32)SavedActiveWeaponSlot2;
                    
                    UE_LOG(LogTemp, Warning, TEXT("AUTO-SAVE: Saved weapons - Slot1=%d, Slot2=%d"), 
                        SaveGameInstance->Weapon1, SaveGameInstance->Weapon2);
                }
                
                UE_LOG(LogTemp, Warning, TEXT("AUTO-SAVE: Buff states - HasRotatingShield=%d, HasReroll=%d"), 
                    PlayerCharacter->bHasRotatingShield, PlayerCharacter->bHasReroll);
                    
                UE_LOG(LogTemp, Log, TEXT("Saving player stats - Health: %f/%f, Speed: %f * %f"),
                    PlayerCharacter->CurrentHealth, PlayerCharacter->MaxHealth,
                    PlayerCharacter->BaseSpeed, PlayerCharacter->SpeedMultiplier);
            }
            
            UE_LOG(LogTemp, Log, TEXT("Auto-saving game: Location = %s, Rotation = %s"),
                *Pawn->GetActorLocation().ToString(), *Pawn->GetActorRotation().ToString());
        }
    }

    // Get the next slot to save to
    FString SlotName = USaveGameFunctions::GetNextSaveSlot();

    // Save the game
    if (UGameplayStatics::SaveGameToSlot(SaveGameInstance, SlotName, 0))
    {
        UE_LOG(LogTemp, Log, TEXT("Auto-saved game to slot %s"), *SlotName);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to auto-save game to slot %s"), *SlotName);
    }
}
void UMyGameInstance::DeleteAllSaveGames()
{
    // Delete all three save slots
    bool bSuccess = true;
    
    if (UGameplayStatics::DoesSaveGameExist(SaveSlot1, 0))
    {
        if (!UGameplayStatics::DeleteGameInSlot(SaveSlot1, 0))
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to delete save game in slot: %s"), *SaveSlot1);
            bSuccess = false;
        }
    }
    
    if (UGameplayStatics::DoesSaveGameExist(SaveSlot2, 0))
    {
        if (!UGameplayStatics::DeleteGameInSlot(SaveSlot2, 0))
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to delete save game in slot: %s"), *SaveSlot2);
            bSuccess = false;
        }
    }
    
    if (UGameplayStatics::DoesSaveGameExist(SaveSlot3, 0))
    {
        if (!UGameplayStatics::DeleteGameInSlot(SaveSlot3, 0))
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to delete save game in slot: %s"), *SaveSlot3);
            bSuccess = false;
        }
    }
    
    // Reset the auto-save slot counter to 1
    CurrentAutoSaveSlot = 1;
    
    if (bSuccess)
    {
        UE_LOG(LogTemp, Log, TEXT("Successfully deleted all save games."));
    }
}

void UMyGameInstance::LoadGlobalSettings()
{
    const FString SlotName = TEXT("GlobalSettingsSlot");
    if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
    {
        UGlobalSettingsSaveGame* GlobalSave = Cast<UGlobalSettingsSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
        GlobalExposure = GlobalSave->GlobalExposure;
    }
}

void UMyGameInstance::SaveGlobalSettings()
{
    const FString SlotName = TEXT("GlobalSettingsSlot");
    UGlobalSettingsSaveGame* GlobalSave = Cast<UGlobalSettingsSaveGame>(UGameplayStatics::CreateSaveGameObject(UGlobalSettingsSaveGame::StaticClass()));
    GlobalSave->GlobalExposure = GlobalExposure;
    UGameplayStatics::SaveGameToSlot(GlobalSave, SlotName, 0);
}

void UMyGameInstance::ApplyGlobalExposure(UWorld* World)
{
    TArray<AActor*> PostProcessVolumes;
    UGameplayStatics::GetAllActorsOfClass(World, APostProcessVolume::StaticClass(), PostProcessVolumes);

    for (AActor* Actor : PostProcessVolumes)
    {
        APostProcessVolume* Volume = Cast<APostProcessVolume>(Actor);
        if (Volume && Volume->bEnabled)
        {
            Volume->Settings.bOverride_AutoExposureBias = true;
            Volume->Settings.AutoExposureBias = GlobalExposure;
        }
    }
}

void UMyGameInstance::LoadGameFromSlot(const FString& SlotName)
{
    // Load the save game
    UInGameSaveGame* LoadedGame = Cast<UInGameSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
    if (!LoadedGame)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load game from slot: %s"), *SlotName);
        return;
    }

    // Set the loading flag to prevent auto-save
    bIsLoadingSaveGame = true;
    
    // Store player stats to apply after level load
    SavedPlayerHealth = LoadedGame->CurrentHealth;
    SavedMaxHealth = LoadedGame->MaxHealth;
    SavedSpeedMultiplier = LoadedGame->SpeedMultiplier;
    SavedBaseSpeed = LoadedGame->BaseSpeed;
    
    // LOAD BUFF STATES
    SavedHasRotatingShield = LoadedGame->bHasRotatingShield;
    SavedHasReroll = LoadedGame->bHasReroll;
    SavedSkillDamage = LoadedGame->SkillDamage;
    SavedBaseSkillCooldownTime = LoadedGame->BaseSkillCooldownTime;
    
    // CRITICAL: Store weapon data from the save file
    SavedWeaponSlot1AsInt = LoadedGame->Weapon1;
    SavedWeaponSlot2AsInt = LoadedGame->Weapon2;
    
    // Also store as enum types
    SavedActiveWeaponSlot1 = static_cast<EWeaponType>(LoadedGame->Weapon1);
    SavedActiveWeaponSlot2 = static_cast<EWeaponType>(LoadedGame->Weapon2);
    
    // Store transform
    SavedPlayerTransform = FTransform(LoadedGame->PlayerRotation, LoadedGame->PlayerLocation);
    
    // Set flags to apply data after level load
    bShouldApplySavedTransform = true;
    bShouldApplySavedPlayerStats = true;
    
    UE_LOG(LogTemp, Warning, TEXT("LoadGameFromSlot: Loaded weapons - Slot1=%d, Slot2=%d"), 
        SavedWeaponSlot1AsInt, SavedWeaponSlot2AsInt);
    
    UE_LOG(LogTemp, Warning, TEXT("LoadGameFromSlot: Loaded buffs - HasRotatingShield=%d, HasReroll=%d"), 
        SavedHasRotatingShield, SavedHasReroll);
    
    // Load the level - Convert FString to FName
    FName LevelName(*LoadedGame->CurrentMap);
    UGameplayStatics::OpenLevel(this, LevelName);
}
