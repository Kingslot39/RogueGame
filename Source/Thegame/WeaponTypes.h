// WeaponTypes.h
#pragma once
#include "CoreMinimal.h"
#include "WeaponTypes.generated.h"

// Weapon type enum - 4 weapons for now, expandable to 6 later
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	WeaponA UMETA(DisplayName = "Weapon A"),
	WeaponB UMETA(DisplayName = "Weapon B"),
	WeaponC UMETA(DisplayName = "Weapon C"),
	WeaponD UMETA(DisplayName = "Weapon D"),
	None UMETA(DisplayName = "None")
};

// Weapon data structure
USTRUCT(BlueprintType)
struct FWeaponData
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FText DisplayName = FText::GetEmpty();
    
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FText Description = FText::GetEmpty();
    
	// The static mesh to use for this weapon
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	UStaticMesh* WeaponMesh = nullptr;
    
	// Location offset from the bone
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Transform")
	FVector LocationOffset = FVector::ZeroVector;
    
	// Rotation offset from the bone
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Transform")
	FRotator RotationOffset = FRotator::ZeroRotator;
    
	// Scale to apply to the weapon
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Transform")
	FVector Scale = FVector(1.0f, 1.0f, 1.0f);
    
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	UTexture2D* WeaponIcon = nullptr;
    
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float Damage = 10.0f;
};
