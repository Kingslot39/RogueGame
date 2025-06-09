// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BushWallSkill.h"
#include "InGameUI.h"
#include "Landmark.h"
#include "LightArrow.h"
#include "LightShieldForce.h"
#include "ShieldForce.h"
#include "GameFramework/Character.h"
#include "WeaponSystem.h"
#include "ComboSkillSystem.h"
#include "HolySwordFallDown.h"
#include "Logging/LogMacros.h"
#include "ThegameCharacter.generated.h"

class ACrossbowArrow;
class AWeaponHitDetector;
class UMyGameInstance;
enum class EWeaponType : uint8;
class AWeaponSystem;
class AComboSkillSystem;
class UInGameUI;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AThegameCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;
	
	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* WeaponSkillAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Weapon2SkillAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* WeaponCombineSkillAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DashingAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SetupAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RewindAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* PauseAction;

public:
	AThegameCharacter();
	
	UPROPERTY(EditAnywhere)
	AActor* SpawnTeleport;
	bool bCanRewind = false;
	bool bAlreadySetup = false;
    void Setup();
	void Rewind();
    UPROPERTY(EditAnywhere)
	TSubclassOf<ALandmark>LandmarkClass;
	
	void SkillActivate();
	void Skill2Activate();
	void SkillCombineActivate();
	
    void ResetSkillCooldown();
	void ResetSkill2Cooldown();
    void SkillShieldDelay();
	
	bool bIsSkillReady = true;
	bool bIsDashing = false;
	void Dashing();
	UPROPERTY(EditAnywhere)
	float DashDistance = 2000;

	UPROPERTY(EditAnywhere)
	UAnimMontage* ShieldSkillAnimation;

	FTimerHandle SkilllShieldDelayHandle;
	FTimerHandle SkillCooldownHandle;
	FTimerHandle CombineCooldownHandle;
	FTimerHandle Skill2CooldownHandle;
	UPROPERTY(EditAnywhere)
	float CooldownTime= 1;
	UPROPERTY(EditAnywhere)
	TSubclassOf<ALightArrow>LightArrowClass;
	UPROPERTY(EditAnywhere)
	float MovementSpeed = 2000;
	
    UPROPERTY(EditAnywhere)
	TSubclassOf<AShieldForce>ShieldForceClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ALightShieldForce>LightShieldForceClass;
	//Sword Fall Down Skill
	UPROPERTY(EditAnywhere)
	TSubclassOf<AHolySwordFallDown>SwordFallDownClass;
	
	//Mouse Corse location
	FRotator Mouse;
	UFUNCTION(BlueprintCallable)
	void GetCharacterRotate();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health")
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Health")
	float CurrentHealth;
	
	UFUNCTION(BlueprintCallable, Category = "Health")
	void GetDamage(float DamageAmount);
	UFUNCTION(BlueprintCallable, Category = "Player Stats")
	void ResetToDefaultStats();
	//Speed Character
	// Speed multiplier for buffs
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats")
	float SpeedMultiplier = 1.0f;
	// Base speed without multipliers
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats")
	float BaseSpeed = 700.0f;
	
	UPROPERTY(EditAnywhere)
	bool CanDashing;
	FTimerHandle DashReset;
	void DashingReset();

	//In Game UI
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetInGameUIVisibility(bool bVisible);
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> InGameUIClass;
	UPROPERTY()
	UInGameUI* InGameUI;
	virtual void Tick(float DeltaSeconds) override;
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* SwordMesh;
	
	UPROPERTY(EditAnywhere)
	UAnimMontage* DashAnimation;
	//Reroll Buff
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bHasReroll = false;

	//Damage DOT
	UFUNCTION(BlueprintCallable)
	void GetDamageDot(float Damage);

	//Healin
	void HealthPlayer(float amount);

	//Niagara
	UPROPERTY(EditAnywhere)
	UNiagaraSystem* TelegraphSystem;
	//Damage
	// Base damage amount (used in blueprints for hit detection)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float BaseDamageAmount = 50.0f;

	// Current damage amount (after buffs applied)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float DamageAmount = 50.0f;

	// Base max health (without buffs)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float BaseMaxHealth = 100.0f;
	// Get current damage amount (for blueprint use)
	UFUNCTION(BlueprintCallable, Category = "Combat")
	float GetCurrentDamage() const { return DamageAmount; }
protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);
	
	// To add mapping context
	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
	// Skill 1 cooldown
	float SkillCooldownTime;  // Total time for the cooldown
	float SkillCooldownProgress;  // Current progress for the cooldown (0.0 to 1.0)
	// Cooldown Timer Handles
	FTimerHandle SkillCooldownTimerHandle;         // Timer for controlling skill availability
	FTimerHandle SkillCooldownUpdateTimerHandle;   // Timer for updating the UI progress smoothly
	void UpdateSkillCooldownProgress();
	
	// Base skill cooldown time
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	float BaseSkillCooldownTime;

	// Skill damage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	float SkillDamage;
	
	//Skill2
	bool bIsSkill2Ready = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills")
	float BaseSkill2CooldownTime = 2.0f;

	void UpdateSkill2CooldownProgress();
	float Skill2CooldownProgress = 1.0f;
	float Skill2CooldownTime; // We’ll set this in the constructor

	// Cooldown and animation controls
	FTimerHandle Skill2CooldownTimerHandle;
	FTimerHandle Skill2CooldownUpdateTimerHandle;
	bool bIsPlayingSkill2Animation = false;
	void ResetSkill2AnimationFlag();
	FTimerHandle Skill2AnimationTimerHandle;
	

	// Handle primary attack input (determines whether to use melee or ranged)
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void HandlePrimaryAttack();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* PrimaryAttackAction;
    //BushWallSkill
	bool bCanRiseWall;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<ABushWallSkill>BushWallClass;
	FTimerHandle BushCoolDownTime;
	void SpawnBushWall();
	void SkillBushAvailable();
	
	
	//Pause
	void HandlePause();
	bool bIsPaused;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> PauseMenuWidgetClass;
	UFUNCTION(BlueprintCallable)
	void ResumeGame();
	
	// Death screen widget class to instantiate when the player dies
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> DeathScreenWidgetClass;

	// Instance of the death screen widget
	UPROPERTY()
	UUserWidget* DeathScreenWidget;

	// For Skill Combine UI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills") 
	float SkillCombineCooldownProgress;   // Cooldown progress for the combine skill (0.0 to 1.0)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills")
	float CombineCooldownTime; // Cooldown duration for the combine skill
	bool bIsSkillComReady = true;
	FTimerHandle CombineCooldownUpdateTimerHandle;
	void SelectWeapons(EWeaponType Weapon1, EWeaponType Weapon2);
	bool IsSkillCombineReady();
	float GetSkillCombineCooldownProgress();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetPlayerLocation(FVector NewLocation);
	float OriginalBrakingDecelerationWalking;
	float OriginalGravityScale;
	void ReApplyGravity();
	UFUNCTION(BlueprintCallable, Category = "Camera")
	void SetFixedCameraLocation(FVector NewLocation, float DeltaTime);

	//For Weapon System
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	AWeaponSystem* WeaponSystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	AComboSkillSystem* ComboSkillSystem;
	// Add these properties to ThegameCharacter.h
	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	TSubclassOf<AWeaponSystem> WeaponSystemClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	TSubclassOf<AComboSkillSystem> ComboSkillSystemClass;

	UFUNCTION(BlueprintCallable, Category = "Weapons")
	void TestWeaponAttachments();
	// Weapon hit detection
	UPROPERTY()
	class AWeaponHitDetector* WeaponHitDetector;

	// Weapon hit detector class
	UPROPERTY(EditDefaultsOnly, Category = "Weapons|Hit Detection")
	TSubclassOf<class AWeaponHitDetector> WeaponHitDetectorClass;

	// Called from animation blueprint or animation notifies when attack starts
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StartAttackHitDetection();

	// Called from animation blueprint or animation notifies when attack ends
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void EndAttackHitDetection();

	// Test function to manually trigger weapon hit detection
	UFUNCTION(BlueprintCallable, Category = "Combat|Testing")
	void TestWeaponHitDetection();

	//Rotating Shield
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
	TSubclassOf<class ARotatingShield> RotatingShieldClass;
	
	UPROPERTY()
	class ARotatingShield* ActiveRotatingShield;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shield")
	bool bHasRotatingShield = false;
	
	UFUNCTION(BlueprintCallable, Category = "Shield")
	void ActivateRotatingShield();
	
	UFUNCTION(BlueprintCallable, Category = "Shield")
	void DeactivateRotatingShield();
	
	
	// Crossbow properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Crossbow")
	TSubclassOf<class ACrossbowArrow> CrossbowArrowClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Crossbow")
	float CrossbowFireRate = 0.5f; // Time between shots

	UPROPERTY(BlueprintReadOnly, Category = "Combat|Crossbow")
	bool bCanFireCrossbow = true;

	// Crossbow functions
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void FireCrossbowArrow();

	// Blueprint event for sword attack - this will trigger your existing blueprint logic
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void OnSwordAttack();

	// Timer handle for crossbow cooldown
	FTimerHandle CrossbowCooldownTimer;

	// Function to reset crossbow cooldown
	void ResetCrossbowCooldown();
	
};



