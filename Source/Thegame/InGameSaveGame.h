// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "InGameSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class THEGAME_API UInGameSaveGame : public USaveGame
{
	GENERATED_BODY()
public:

	UPROPERTY(VisibleAnywhere, Category = "Settings")
	FString SaveSlotName = "SettingsSlot";

	UPROPERTY(VisibleAnywhere, Category = "Settings")
	int32 UserIndex = 0;

	// New properties for level and player transform
	UPROPERTY(VisibleAnywhere, Category = "Settings")
	FString CurrentMap; // The name of the map when the game was saved

	UPROPERTY(VisibleAnywhere, Category = "Settings")
	FVector PlayerLocation; // The player's saved location

	UPROPERTY(VisibleAnywhere, Category = "Settings")
	FRotator PlayerRotation; // The player's saved rotation
	
	// Timestamp for sorting
	UPROPERTY(VisibleAnywhere, Category = "Basic")
	FString Timestamp;
	// Health
	UPROPERTY(VisibleAnywhere) float CurrentHealth;
	UPROPERTY(VisibleAnywhere) float MaxHealth;

	// Movement
	UPROPERTY(VisibleAnywhere) float SpeedMultiplier;
	UPROPERTY(VisibleAnywhere) float BaseSpeed;

	//Weapons
	// Weapons - stored as integers (matching the EWeaponType enum values)
	UPROPERTY(VisibleAnywhere, Category = "Player Stats") 
	int32 Weapon1 = 0; // Default to None (0)

	UPROPERTY(VisibleAnywhere, Category = "Player Stats") 
	int32 Weapon2 = 0; // Default to None (0)

	// Buff states
	UPROPERTY(VisibleAnywhere, Category = "Buffs")
	bool bHasRotatingShield = false;
    
	UPROPERTY(VisibleAnywhere, Category = "Buffs")
	bool bHasReroll = false;
    
	UPROPERTY(VisibleAnywhere, Category = "Buffs")
	float SkillDamage = 50.0f;
    
	UPROPERTY(VisibleAnywhere, Category = "Buffs")
	float BaseSkillCooldownTime = 3.0f;
};
