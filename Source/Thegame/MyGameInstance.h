// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "WeaponSystem.h"
#include "MyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class THEGAME_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	// Stored transform to reposition the player after level load
	UPROPERTY(BlueprintReadWrite, Category="SaveGame")
	FTransform SavedPlayerTransform;

	// Flag indicating whether we should apply the saved transform on level load
	UPROPERTY(BlueprintReadWrite, Category="SaveGame")
	bool bShouldApplySavedTransform = false;

	UPROPERTY(BlueprintReadWrite, Category = "SaveGame")
	bool bStageClear = false;
	
	// Variable to store the name of the previous map.
	UPROPERTY(BlueprintReadWrite, Category = "SaveGame")
	FString PreviousMapName;
	
	// Flag indicating that the level is being loaded from a saved file.
	UPROPERTY(BlueprintReadWrite, Category = "SaveGame")
	bool bIsLoadingSaveGame = false;
	
	UPROPERTY(BlueprintReadWrite, Category = "SaveGame")
	FString LastAutoSavedMapName;
	// Function to perform auto-save.
	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	void AutoSaveGame();
    
	// Called after a new level has loaded.
	void OnPostLoadMap(UWorld* LoadedWorld);
    
	virtual void Init() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UInGameSetting> InGameSettingClass;

	// Fixed save slot names
	static const FString SaveSlot1;
	static const FString SaveSlot2;
	static const FString SaveSlot3;
	static int32 CurrentAutoSaveSlot;
	
	// Function to delete all save files (for new game)
	UFUNCTION(BlueprintCallable, Category = "Save Game")
	void DeleteAllSaveGames();
	// Load global settings on game start
	// Add this declaration
	UPROPERTY(BlueprintReadWrite, Category = "Global Settings")
	float GlobalExposure = 1.5f; // Default value
	float CurrentExposure = 1.5f; // Default value
	void LoadGlobalSettings();
	void SaveGlobalSettings();
	void ApplyGlobalExposure(UWorld* World);

	UPROPERTY()
	float SavedPlayerHealth;

	UPROPERTY()
	float SavedMaxHealth;

	UPROPERTY()
	float SavedSpeedMultiplier;

	UPROPERTY()
	float SavedBaseSpeed;

	UPROPERTY()
	bool bShouldApplySavedPlayerStats;

	// Weapon system variables
	UPROPERTY(BlueprintReadWrite, Category = "Player Stats")
	EWeaponType SavedActiveWeaponSlot1;

	UPROPERTY(BlueprintReadWrite, Category = "Player Stats")
	EWeaponType SavedActiveWeaponSlot2;

	UPROPERTY(BlueprintReadWrite, Category = "Player Stats")
	int32 SavedWeaponSlot1AsInt = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Player Stats")
	int32 SavedWeaponSlot2AsInt = 0;

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	void LoadGameFromSlot(const FString& SlotName);

	// Saved buff states
	UPROPERTY(BlueprintReadWrite, Category = "Player Stats")
	bool SavedHasRotatingShield = false;
    
	UPROPERTY(BlueprintReadWrite, Category = "Player Stats")
	bool SavedHasReroll = false;
    
	UPROPERTY(BlueprintReadWrite, Category = "Player Stats")
	float SavedSkillDamage = 0.0f;
    
	UPROPERTY(BlueprintReadWrite, Category = "Player Stats")
	float SavedBaseSkillCooldownTime = 0.0f;
};
