// WeaponSystem.h
#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponTypes.h"
#include "WeaponSystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponsChangedDelegate, EWeaponType, FirstWeapon, EWeaponType, SecondWeapon);

UCLASS(BlueprintType, Blueprintable)
class THEGAME_API AWeaponSystem : public AActor
{
    GENERATED_BODY()
    
public:    
    AWeaponSystem();
    // Update weapon attachments
    void UpdateWeaponAttachments();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    
    // Weapon data for each type
    UPROPERTY(EditDefaultsOnly, Category = "Weapons|A")
    FWeaponData WeaponA;
    
    UPROPERTY(EditDefaultsOnly, Category = "Weapons|B")
    FWeaponData WeaponB;
    
    UPROPERTY(EditDefaultsOnly, Category = "Weapons|C")
    FWeaponData WeaponC;
    
    UPROPERTY(EditDefaultsOnly, Category = "Weapons|D")
    FWeaponData WeaponD;
    
    // Currently selected weapons
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapons")
    EWeaponType ActiveWeaponSlot1;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapons")
    EWeaponType ActiveWeaponSlot2;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapons")
    EWeaponType ActiveWeaponSlot3;
    
    UPROPERTY(VisibleAnywhere, Category = "Components")
    UStaticMeshComponent* WeaponMesh1;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UStaticMeshComponent* WeaponMesh2;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UStaticMeshComponent* WeaponMesh3;
    
    // Add owner reference to manually track
    UPROPERTY()
    AActor* OwningActor;
    
    // Delegate fired when weapons change
    UPROPERTY(BlueprintAssignable, Category = "Weapons")
    FOnWeaponsChangedDelegate OnWeaponsChanged;
    
    // Get weapon data for a specific weapon type
    UFUNCTION(BlueprintCallable, Category = "Weapons")
    FWeaponData GetWeaponData(EWeaponType WeaponType) const;
    
    // Select weapons to equip
    UFUNCTION(BlueprintCallable, Category = "Weapons")
    void SelectWeapons(EWeaponType Weapon1, EWeaponType Weapon2);
    
    // Set the owning actor
    UFUNCTION(BlueprintCallable, Category = "Setup")
    void SetOwningActor(AActor* NewOwner);
    
    // Determine if a weapon type is valid (has mesh assigned)
    UFUNCTION(BlueprintCallable, Category = "Weapons")
    bool IsWeaponTypeValid(EWeaponType WeaponType) const;
    
    // Check if a weapon is an attack type (A or C)
    UFUNCTION(BlueprintCallable, Category = "Weapons")
    bool IsAttackWeapon(EWeaponType WeaponType) const;
    
    // Check if a weapon is a support type (B or D)
    UFUNCTION(BlueprintCallable, Category = "Weapons")
    bool IsSupportWeapon(EWeaponType WeaponType) const;
    
    // Log weapon data status for debugging
    UFUNCTION()
    void LogWeaponDataStatus();
    
    // Apply weapon buffs based on the equipped weapons
    UFUNCTION(BlueprintCallable, Category = "Weapons")
    void ApplyWeaponBuffs();
    
    // Update weapon meshes based on selected weapons
    void UpdateWeaponMeshes();
    // Get current buff values - useful for UI or other systems
    UFUNCTION(BlueprintCallable, Category = "Weapons|Buffs")
    float GetDamageMultiplier() const { return CurrentDamageMultiplier; }
    
    UFUNCTION(BlueprintCallable, Category = "Weapons|Buffs")
    float GetHealthBonus() const { return CurrentHealthBonus; }
    
    UFUNCTION(BlueprintCallable, Category = "Weapons|Buffs")
    float GetSpeedMultiplier() const { return CurrentSpeedMultiplier; }
private:
    // Clear any applied weapon buffs
    void ClearWeaponBuffs();
    
    // Current buff values
    float CurrentDamageMultiplier = 1.0f;
    float CurrentHealthBonus = 0.0f;
    float CurrentSpeedMultiplier = 1.0f;
    
   
};