// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InGameUI.h"
#include "Landmark.h"
#include "LightArrow.h"
#include "LightShieldForce.h"
#include "ShieldForce.h"
#include "Engine/InGameAdManager.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "ThegameCharacter.generated.h"

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
	void ResetCombineCooldown();
    void SkillShieldDelay();
	
	bool bIsSkillReady = true;
	
	bool bIsSkillComReady = true;

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

	UFUNCTION(BlueprintCallable)
	void GetCharacterRotate();

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health")
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Health")
	float CurrentHealth;
	
	UFUNCTION(BlueprintCallable, Category = "Health")
	void GetDamage(float DamageAmount);



	UPROPERTY(EditAnywhere)
	bool CanDashing;
	FTimerHandle DashReset;
	void DashingReset();

	//In Game UI
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

	//For mana
	float MaxMana;          // Maximum mana for the character
	float CurrentMana;      // Current mana value
	float ManaRegenRate;    // Rate of mana regeneration per second
	float ManaRegenDelay;   // Delay before mana starts regenerating after using a skill
	FTimerHandle ManaRegenTimerHandle; // Timer handle for mana regeneration

	

	//FOR SKILL COOLDOWN
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
};



