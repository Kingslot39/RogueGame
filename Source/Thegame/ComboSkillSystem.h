#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponTypes.h"  // Include to access weapon types
#include "ComboSkillSystem.generated.h"

// Forward declarations
class UInGameUI;
class AWeaponSystem;

// Struct to hold combo skill data
USTRUCT(BlueprintType)
struct FComboSkillData
{
    GENERATED_BODY()
    
    // Name of the combo
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
    FString ComboName;
    
    // Damage multiplier for the combo
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
    float DamageMultiplier = 1.5f;
    
    // Cooldown in seconds
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
    float Cooldown = 10.0f;
    
    // Icon for the combo
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
    UTexture2D* ComboIcon;
    
    // Effect Blueprint to spawn when combo is used
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
    TSubclassOf<AActor> ComboEffect;
    
    // Current cooldown time remaining
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combo")
    float CooldownRemaining = 0.0f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnComboActivatedDelegate, FString, ComboName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnComboCooldownChangedDelegate, FString, ComboName, float, CooldownPercentage);

UCLASS(BlueprintType, Blueprintable)
class THEGAME_API AComboSkillSystem : public AActor
{
    GENERATED_BODY()
    
public:    
    AComboSkillSystem();
    
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    
    // Weapon System reference
    UPROPERTY()
    AWeaponSystem* WeaponSystem;
    
    // Combo Skill Data for different weapon combinations
    UPROPERTY(EditDefaultsOnly, Category = "Combos")
    FComboSkillData ComboAB;  // Combo for Weapon A + Weapon B
    
    UPROPERTY(EditDefaultsOnly, Category = "Combos")
    FComboSkillData ComboAC;  // Combo for Weapon A + Weapon C
    
    UPROPERTY(EditDefaultsOnly, Category = "Combos")
    FComboSkillData ComboAD;  // Combo for Weapon A + Weapon D
    
    UPROPERTY(EditDefaultsOnly, Category = "Combos")
    FComboSkillData ComboBC;  // Combo for Weapon B + Weapon C
    
    UPROPERTY(EditDefaultsOnly, Category = "Combos")
    FComboSkillData ComboBD;  // Combo for Weapon B + Weapon D
    
    UPROPERTY(EditDefaultsOnly, Category = "Combos")
    FComboSkillData ComboCD;  // Combo for Weapon C + Weapon D
    
    // Owning actor reference
    UPROPERTY()
    AActor* OwningActor;
    
    // In-game UI reference
    UPROPERTY()
    UInGameUI* InGameUI;
    
    // Combo activation delegates
    UPROPERTY(BlueprintAssignable, Category = "Combo")
    FOnComboActivatedDelegate OnComboActivated;
    
    UPROPERTY(BlueprintAssignable, Category = "Combo")
    FOnComboCooldownChangedDelegate OnComboCooldownChanged;
    
    // Set the owner actor
    UFUNCTION(BlueprintCallable, Category = "Setup")
    void SetOwningActor(AActor* NewOwner);
    
    // Set the weapon system reference
    UFUNCTION(BlueprintCallable, Category = "Setup")
    void SetWeaponSystem(AWeaponSystem* InWeaponSystem);
    
    // Set the UI reference
    UFUNCTION(BlueprintCallable, Category = "UI")
    void SetInGameUI(UInGameUI* UI);
    
    // Try to activate a combo based on current weapons
    UFUNCTION(BlueprintCallable, Category = "Combo")
    bool TryActivateCombo();
    
    // Get combo data for a specific weapon combination
    UFUNCTION(BlueprintCallable, Category = "Combo")
    FComboSkillData GetComboData(EWeaponType Weapon1, EWeaponType Weapon2);
    
    // Check if a combo is ready (not on cooldown)
    UFUNCTION(BlueprintCallable, Category = "Combo")
    bool IsComboReady(EWeaponType Weapon1, EWeaponType Weapon2);
    
    // Activate a specific combo directly
    UFUNCTION(BlueprintCallable, Category = "Combo")
    bool ActivateCombo(EWeaponType Weapon1, EWeaponType Weapon2);
    
    UFUNCTION(BlueprintCallable, Category = "Combo")
    float GetComboCooldownProgress(EWeaponType Weapon1, EWeaponType Weapon2);
private:
    // Update cooldowns on all combos
    void UpdateCooldowns(float DeltaTime);
    
    // Activate the effect for a combo
    void ActivateComboEffect(FComboSkillData& ComboData);
    
    // Get a string identifier for a weapon pair (for debugging)
    FString GetComboNameForWeapons(EWeaponType Weapon1, EWeaponType Weapon2);

    FComboSkillData* GetComboDataPtr(EWeaponType Weapon1, EWeaponType Weapon2);

};
