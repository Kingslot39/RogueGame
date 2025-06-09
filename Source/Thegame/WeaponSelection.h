// WeaponSelectionUI.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponTypes.h"
#include "WeaponSelection.generated.h"

class UButton;
class UImage;
class UTextBlock;
class AThegameCharacter;
class AWeaponSystem;

/**
 * UI Widget for weapon selection after NPC interaction
 */
UCLASS()
class THEGAME_API UWeaponSelectionUI : public UUserWidget
{
    GENERATED_BODY()
    
public:
    // Initialize the widget with the player character
    UFUNCTION(BlueprintCallable, Category = "Weapon Selection")
    void SetupWeaponSelection(AThegameCharacter* PlayerCharacter);
    
    // Close the weapon selection UI
    UFUNCTION(BlueprintCallable, Category = "Weapon Selection")
    void CloseWeaponSelection();
    
    // Confirm the weapon selection and apply it to the player
    UFUNCTION(BlueprintCallable, Category = "Weapon Selection")
    void ConfirmWeaponSelection();
    
    // Select an attack weapon (A or C)
    UFUNCTION(BlueprintCallable, Category = "Weapon Selection")
    void SelectAttackWeapon(EWeaponType WeaponType);
    
    // Select a support weapon (B or D)
    UFUNCTION(BlueprintCallable, Category = "Weapon Selection")
    void SelectSupportWeapon(EWeaponType WeaponType);
    
    // Update the preview of the selected weapons
    UFUNCTION(BlueprintCallable, Category = "Weapon Selection")
    void UpdateWeaponPreview();
    
    // Add these to the class properties section in WeaponSelectionUI.h
    UPROPERTY(meta = (BindWidget))
    UTextBlock* BaseHPText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* BaseSpeedText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* BaseAttackText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* BuffHPText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* BuffSpeedText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* BuffAttackText;

    // Add a new method to update stat displays
    UFUNCTION()
    void UpdateStatDisplay();
protected:
    virtual void NativeConstruct() override;
    
    // Button click handlers
    UFUNCTION()
    void OnWeaponAClicked();
    
    UFUNCTION()
    void OnWeaponBClicked();
    
    UFUNCTION()
    void OnWeaponCClicked();
    
    UFUNCTION()
    void OnWeaponDClicked();
    
    UFUNCTION()
    void OnConfirmClicked();
    
    UFUNCTION()
    void OnCancelClicked();
    
    // The player character reference
    UPROPERTY()
    AThegameCharacter* Player;
    
    // The weapon system reference
    UPROPERTY()
    AWeaponSystem* WeaponSystem;
    
    // Currently selected weapons
    UPROPERTY()
    EWeaponType SelectedAttackWeapon;
    
    UPROPERTY()
    EWeaponType SelectedSupportWeapon;
    
    // UI Elements - bind these in your Blueprint widget
    UPROPERTY(meta = (BindWidget))
    UButton* WeaponAButton;
    
    UPROPERTY(meta = (BindWidget))
    UButton* WeaponBButton;
    
    UPROPERTY(meta = (BindWidget))
    UButton* WeaponCButton;
    
    UPROPERTY(meta = (BindWidget))
    UButton* WeaponDButton;
    
    UPROPERTY(meta = (BindWidget))
    UButton* ConfirmButton;
    
    UPROPERTY(meta = (BindWidget))
    UButton* CancelButton;
    
    UPROPERTY(meta = (BindWidget))
    UImage* AttackWeaponPreview;
    
    UPROPERTY(meta = (BindWidget))
    UImage* SupportWeaponPreview;
    
    UPROPERTY(meta = (BindWidget))
    UTextBlock* AttackWeaponName;
    
    UPROPERTY(meta = (BindWidget))
    UTextBlock* SupportWeaponName;
    
    UPROPERTY(meta = (BindWidget))
    UTextBlock* ComboNameText;
};