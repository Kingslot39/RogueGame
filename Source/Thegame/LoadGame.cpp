// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadGame.h"
#include "Kismet/GameplayStatics.h"
#include "InGameSaveGame.h"
#include "InGameSetting.h"
#include "Engine/World.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "MyGameInstance.h"
#include "SaveGameFunctions.h"
#include "ThegameCharacter.h"
#include "Logging/LogMacros.h"

void ULoadGame::NativeConstruct()
{
    Super::NativeConstruct();

    // Bind the button click events
    if (LoadSlot1Button)
    {
        LoadSlot1Button->OnClicked.AddDynamic(this, &ULoadGame::OnLoadSlot1Clicked);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("LoadSlot1Button is not bound!"));
    }
    
    if (LoadSlot2Button)
    {
        LoadSlot2Button->OnClicked.AddDynamic(this, &ULoadGame::OnLoadSlot2Clicked);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("LoadSlot2Button is not bound!"));
    }
    
    if (LoadSlot3Button)
    {
        LoadSlot3Button->OnClicked.AddDynamic(this, &ULoadGame::OnLoadSlot3Clicked);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("LoadSlot3Button is not bound!"));
    }
    
    if (BackButton)
    {
        BackButton->OnClicked.AddDynamic(this, &ULoadGame::OnBackButtonClicked);
    }
    
    // Create the in-game settings widget if needed
    if (InGameSettingClass)
    {
        if (UInGameSetting* InGameSettingWidget = CreateWidget<UInGameSetting>(this, InGameSettingClass))
        {
            InGameSettingWidget->AddToViewport();
        }
    }
    
    // Update the display for all save slots
    UpdateSlotInfoDisplay();
}

void ULoadGame::UpdateSlotInfoDisplay()
{
    // Update info for each save slot
    UpdateSlotInfo(USaveGameFunctions::SaveSlot1, Slot1TimeText, Slot1MapText);
    UpdateSlotInfo(USaveGameFunctions::SaveSlot2, Slot2TimeText, Slot2MapText);
    UpdateSlotInfo(USaveGameFunctions::SaveSlot3, Slot3TimeText, Slot3MapText);
}

void ULoadGame::UpdateSlotInfo(const FString& SlotName, UTextBlock* TimeText, UTextBlock* MapText)
{
    // Check if the UI elements are valid
    if (!TimeText || !MapText)
    {
        UE_LOG(LogTemp, Error, TEXT("Text blocks for slot %s are not properly bound!"), *SlotName);
        return;
    }
    
    // Check if the save game exists
    if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
    {
        // Load the save game to get its details
        UInGameSaveGame* SaveGame = Cast<UInGameSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
        if (SaveGame)
        {
            // Update the text blocks with the save game details
            TimeText->SetText(FText::FromString(SaveGame->Timestamp));
            MapText->SetText(FText::FromString(SaveGame->CurrentMap));
            UE_LOG(LogTemp, Log, TEXT("Slot %s: Map=%s, Time=%s"), *SlotName, *SaveGame->CurrentMap, *SaveGame->Timestamp);
        }
    }
    else
    {
        // No save game in this slot
        TimeText->SetText(FText::FromString("Empty Slot"));
        MapText->SetText(FText::FromString("No Save Data"));
        UE_LOG(LogTemp, Log, TEXT("Slot %s: Empty"), *SlotName);
    }
}

void ULoadGame::OnLoadSlot1Clicked()
{
    LoadSaveGame(USaveGameFunctions::SaveSlot1);
}

void ULoadGame::OnLoadSlot2Clicked()
{
    LoadSaveGame(USaveGameFunctions::SaveSlot2);
}

void ULoadGame::OnLoadSlot3Clicked()
{
    LoadSaveGame(USaveGameFunctions::SaveSlot3);
}

void ULoadGame::OnBackButtonClicked()
{
    // Remove this widget from the viewport
    RemoveFromParent();
}

void ULoadGame::LoadSaveGame(const FString& SlotName)
{
    // Check if the save game exists
    if (!UGameplayStatics::DoesSaveGameExist(SlotName, 0))
    {
        UE_LOG(LogTemp, Warning, TEXT("No save game found in slot %s"), *SlotName);
        return;
    }
    
    // Get the game instance and use the custom load method
    UMyGameInstance* GameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (GameInstance)
    {
        UE_LOG(LogTemp, Log, TEXT("Using custom LoadGameFromSlot for slot: %s"), *SlotName);
        
        // Close the load screen first
        RemoveFromParent();
        
        // Use the custom load method that properly handles weapon data
        GameInstance->LoadGameFromSlot(SlotName);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to get MyGameInstance"));
    }
}

void ULoadGame::ApplyLoadedGame(UInGameSaveGame* LoadedGame)
{
    // Retrieve the saved map name and current map name
    FString SavedMap = LoadedGame->CurrentMap;
    FString CurrentMap = UGameplayStatics::GetCurrentLevelName(GetWorld());

    // Get the game instance
    UMyGameInstance* GI = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (GI)
    {
        // Apply exposure immediately if staying in the same map
        if (GI->GlobalExposure != 0.0f)
        {
            GI->ApplyGlobalExposure(GetWorld());
        }
    }
    
    // Get player character to update stats if we're staying in the same level
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    AThegameCharacter* PlayerCharacter = nullptr;
    
    if (PC)
    {
        PlayerCharacter = Cast<AThegameCharacter>(PC->GetPawn());
    }
    
    if (SavedMap != CurrentMap)
    {
        // If the saved map is different, prepare for level transition
        UE_LOG(LogTemp, Log, TEXT("Switching from current map %s to saved map %s"), *CurrentMap, *SavedMap);

        if (GI)
        {
            // Store the transform and set the flag to apply it after the level loads
            GI->SavedPlayerTransform = FTransform(LoadedGame->PlayerRotation, LoadedGame->PlayerLocation);
            GI->bShouldApplySavedTransform = true;
            
            // Store player stats to apply after level loads
            GI->SavedPlayerHealth = LoadedGame->CurrentHealth;
            GI->SavedMaxHealth = LoadedGame->MaxHealth;
            GI->SavedSpeedMultiplier = LoadedGame->SpeedMultiplier;
            GI->SavedBaseSpeed = LoadedGame->BaseSpeed;
            GI->bShouldApplySavedPlayerStats = true;
            
            // Mark that we are loading from a save so that auto-save is skipped
            GI->bIsLoadingSaveGame = true;
            UE_LOG(LogTemp, Log, TEXT("Loading from save file; auto-save will be skipped on level load."));
        }

        // Open the saved level
        UGameplayStatics::OpenLevel(GetWorld(), FName(*SavedMap));
    }
    else
    {
        // If already in the correct map, just reposition the player and update stats
        if (PlayerCharacter)
        {
            // Apply position and rotation
            PlayerCharacter->SetActorLocation(LoadedGame->PlayerLocation);
            PlayerCharacter->SetActorRotation(LoadedGame->PlayerRotation);
            
            // Apply saved character stats
            PlayerCharacter->CurrentHealth = LoadedGame->CurrentHealth;
            PlayerCharacter->MaxHealth = LoadedGame->MaxHealth;
            PlayerCharacter->SpeedMultiplier = LoadedGame->SpeedMultiplier;
            PlayerCharacter->BaseSpeed = LoadedGame->BaseSpeed;
            
            // Update the UI to reflect these changes
            if (PlayerCharacter->InGameUI)
            {
                PlayerCharacter->InGameUI->UpdateValues();
                
                // Update rewind button state
                if (PlayerCharacter->bAlreadySetup)
                {
                    PlayerCharacter->InGameUI->SetSetupRewindText("R");
                }
                else
                {
                    PlayerCharacter->InGameUI->SetSetupRewindText("E");
                }
            }
            
            UE_LOG(LogTemp, Log, TEXT("Player repositioned and stats updated. Health: %f, Speed Multiplier: %f"), 
                LoadedGame->CurrentHealth, LoadedGame->SpeedMultiplier);
        }
    }
}