// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoreMinimal.h"
#include "InGameSaveGame.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "LoadingScreen.h" 
#include "Components/TextBlock.h" 
#include "LoadGame.generated.h"

/**
 * 
 */
UCLASS()
class THEGAME_API ULoadGame : public UUserWidget
{
	GENERATED_BODY()
public:

	// Function to load the selected save game
	void LoadSaveGame(const FString& SlotName);
	void ApplyLoadedGame(UInGameSaveGame* LoadedGame);
	virtual void NativeConstruct();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UInGameSetting> InGameSettingClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<ULoadingScreen> LoadingScreenClass;
	// UI Components for the 3 slots
	UPROPERTY(meta = (BindWidget))
	UButton* LoadSlot1Button;
    
	UPROPERTY(meta = (BindWidget))
	UButton* LoadSlot2Button;
    
	UPROPERTY(meta = (BindWidget))
	UButton* LoadSlot3Button;
	
	// Text blocks for displaying slot info
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Slot1TimeText;
    
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Slot1MapText;
    
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Slot2TimeText;
    
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Slot2MapText;
    
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Slot3TimeText;
    
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Slot3MapText;

	// Back button
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UButton* BackButton;
    
	// Button click handlers
	UFUNCTION()
	void OnLoadSlot1Clicked();
    
	UFUNCTION()
	void OnLoadSlot2Clicked();
    
	UFUNCTION()
	void OnLoadSlot3Clicked();
    
	UFUNCTION()
	void OnBackButtonClicked();

	void UpdateSlotInfoDisplay();
	void UpdateSlotInfo(const FString& SlotName, UTextBlock* TimeText, UTextBlock* MapText);

private:
	// Helper function to get the list of available save files
	TArray<FString> GetAvailableSaveGames();

	TMap<FString, FString> SaveSlotMapping;
};
