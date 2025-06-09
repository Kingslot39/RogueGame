// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveGameFunctions.h"

// SaveGameFunctions.cpp

#include "InGameSaveGame.h"
#include "MyGameInstance.h"
#include "WeaponTypes.h"
#include "Kismet/GameplayStatics.h"

// Initialize static constants
const FString USaveGameFunctions::SaveSlot1 = TEXT("SaveSlot_1");
const FString USaveGameFunctions::SaveSlot2 = TEXT("SaveSlot_2");
const FString USaveGameFunctions::SaveSlot3 = TEXT("SaveSlot_3");
int32 USaveGameFunctions::CurrentSaveSlotIndex = 1;

void USaveGameFunctions::DeleteAllSaveGames()
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
    
    // Reset the save slot counter
    CurrentSaveSlotIndex = 1;
    
    if (bSuccess)
    {
        UE_LOG(LogTemp, Log, TEXT("Successfully deleted all save games."));
    }
}

FString USaveGameFunctions::GetNextSaveSlot()
{
    // Get the next save slot in the rotation
    FString SlotName;
    
    switch(CurrentSaveSlotIndex)
    {
        case 1:
            SlotName = SaveSlot1;
            break;
        case 2:
            SlotName = SaveSlot2;
            break;
        case 3:
            SlotName = SaveSlot3;
            break;
        default:
            SlotName = SaveSlot1;
            break;
    }
    
    // Update for next time
    CurrentSaveSlotIndex = (CurrentSaveSlotIndex % 3) + 1;
    
    return SlotName;
}
bool USaveGameFunctions::LoadGameFromSlot(const FString& SlotName, UObject* WorldContextObject)
{
    if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
    {
        // Load the save game data
        UInGameSaveGame* LoadedGame = Cast<UInGameSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
        if (LoadedGame)
        {
            UE_LOG(LogTemp, Log, TEXT("Loaded save game from slot: %s"), *SlotName);
            
            // Get the game instance to store player stats
            UMyGameInstance* GI = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject->GetWorld()));
            if (GI)
            {
                // Store player stats to be applied after level load
                GI->SavedPlayerHealth = LoadedGame->CurrentHealth;
                GI->SavedMaxHealth = LoadedGame->MaxHealth;
                GI->SavedSpeedMultiplier = LoadedGame->SpeedMultiplier;
                GI->SavedBaseSpeed = LoadedGame->BaseSpeed;
                
                // Set transform to be applied
                GI->SavedPlayerTransform.SetLocation(LoadedGame->PlayerLocation);
                GI->SavedPlayerTransform.SetRotation(LoadedGame->PlayerRotation.Quaternion());
                GI->bShouldApplySavedTransform = true;
                GI->bShouldApplySavedPlayerStats = true;
                
                // NEW: Load weapon slots
                GI->SavedActiveWeaponSlot1 = (EWeaponType)LoadedGame->Weapon1;
                GI->SavedActiveWeaponSlot2 = (EWeaponType)LoadedGame->Weapon2;
                
                UE_LOG(LogTemp, Log, TEXT("Loaded weapons: Slot1=%d, Slot2=%d"), 
                    LoadedGame->Weapon1, LoadedGame->Weapon2);
                
                // Set the flag to indicate we're loading a save
                GI->bIsLoadingSaveGame = true;
                
                // Open the saved level
                UGameplayStatics::OpenLevel(WorldContextObject->GetWorld(), FName(*LoadedGame->CurrentMap));
                return true;
            }
        }
    }
    
    UE_LOG(LogTemp, Warning, TEXT("Failed to load save game from slot: %s"), *SlotName);
    return false;
}