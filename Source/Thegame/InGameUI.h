// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "InGameUI.generated.h"

/**
 * 
 */
class AThegameCharacter;
class UUBuffSelectionWidget;
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
	// Skill 1 UI
	UPROPERTY(meta = (BindWidget))
	UProgressBar* Skill1CooldownBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Skill1CooldownText;

	// Skill 2 UI
	UPROPERTY(meta = (BindWidget))
	UProgressBar* Skill2CooldownBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Skill2CooldownText;

	// Skill Combine Ui
	UPROPERTY(meta = (BindWidget))
	UProgressBar* SkillCombineCooldownBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SkillCombineCooldownText;
	
	UPROPERTY()
	AThegameCharacter* Player;

	void UpdateSkillValues(float Skill1Progress, float Skill2Progress, float SkillCombineProgress);
	void UpdateValues();

	//SetupRewind
	// Skill SetupRewind UI
	UPROPERTY(meta = (BindWidget))
	UProgressBar* SetupRewindCooldownBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SetupRewindCooldownText;
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetSetupRewindText(FString NewText);

	
};
