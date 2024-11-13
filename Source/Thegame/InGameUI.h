// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "InGameUI.generated.h"

/**
 * 
 */
class AThegameCharacter;
UCLASS()
class THEGAME_API UInGameUI : public UUserWidget
{
public:
	GENERATED_BODY()
	//Health
	UPROPERTY(meta=(BindWidget))
	UProgressBar* HealthBar;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* CurrentHealthText;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* MaxHealthText;
	//Skill1
	UPROPERTY(meta = (BindWidget))
    UTextBlock* Skill1CooldownText;
	UPROPERTY(meta = (BindWidget))
	UProgressBar* Skill1CooldownBar;
	// Skill2
	UPROPERTY(meta = (BindWidget))
	UProgressBar* Skill2CooldownBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Skill2CooldownText;
	void UpdateSkill2Cooldown(float ProgressValue, UProgressBar* Skill2CooldownBar);
	UPROPERTY()
	AThegameCharacter* Player;
	void UpdateValues();
	void UpdateSkillCooldown(float ProgressValue, UProgressBar* SkillCooldownBar);
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnPlayerDeath();
};
