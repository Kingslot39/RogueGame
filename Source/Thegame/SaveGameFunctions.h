// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SaveGameFunctions.generated.h"

/**
 * 
 */
UCLASS()
class THEGAME_API USaveGameFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Function to delete all save games
	UFUNCTION(BlueprintCallable, Category = "Save Game")
	static void DeleteAllSaveGames();
    
	// Function to get next available save slot
	UFUNCTION(BlueprintCallable, Category = "Save Game")
	static FString GetNextSaveSlot();
    
	// Static constants for save slot names
	static const FString SaveSlot1;
	static const FString SaveSlot2;
	static const FString SaveSlot3;
	
	UFUNCTION(BlueprintCallable, Category = "Save Game")
	static bool LoadGameFromSlot(const FString& SlotName, UObject* WorldContextObject);
private:
	// Track which save slot to use next
	static int32 CurrentSaveSlotIndex;
};
