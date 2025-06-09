// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameUI.h"
#include "ThegameCharacter.h"
#include "Components/ProgressBar.h"

void UInGameUI::UpdateSkillValues(float Skill1Progress, float Skill2Progress, float SkillCombineProgress)
{
	// Log the values we're about to use
    UE_LOG(LogTemp, Verbose, TEXT("UpdateSkillValues: Skill1=%.2f, Skill2=%.2f, Combined=%.2f"), 
           Skill1Progress, Skill2Progress, SkillCombineProgress);

    if (Skill1CooldownBar)
    {
        Skill1CooldownBar->SetPercent(Skill1Progress);

        // Optionally adjust color based on progress
        FLinearColor StartColor = FLinearColor::Gray;
        FLinearColor EndColor = FLinearColor::Blue;
        FLinearColor CurrentColor = FMath::Lerp(StartColor, EndColor, Skill1Progress);
        Skill1CooldownBar->SetFillColorAndOpacity(CurrentColor);
    }

    if (Skill1CooldownText)
    {
        // Hard-code the display to show the key input (X for Skill 1)
        Skill1CooldownText->SetText(FText::FromString(TEXT("X")));
    }

    if (Skill2CooldownBar)
    {
        Skill2CooldownBar->SetPercent(Skill2Progress);

        // Optionally adjust color based on progress
        FLinearColor StartColor = FLinearColor::Gray;
        FLinearColor EndColor = FLinearColor::Green;
        FLinearColor CurrentColor = FMath::Lerp(StartColor, EndColor, Skill2Progress);
        Skill2CooldownBar->SetFillColorAndOpacity(CurrentColor);
    }

    if (Skill2CooldownText)
    {
        // Hard-code the display to show the key input (C for Skill 2)
        Skill2CooldownText->SetText(FText::FromString(TEXT("C")));
    }
    
    if (SkillCombineCooldownBar)
    {
        // Ensure the value is valid (0.0-1.0)
        float ClampedProgress = FMath::Clamp(SkillCombineProgress, 0.0f, 1.0f);
        
        if (ClampedProgress != SkillCombineProgress)
        {
            UE_LOG(LogTemp, Warning, TEXT("Combined skill progress was outside valid range: %.2f, clamped to %.2f"), 
                   SkillCombineProgress, ClampedProgress);
        }
        
        SkillCombineCooldownBar->SetPercent(ClampedProgress);

        // Optionally adjust color based on progress
        FLinearColor StartColor = FLinearColor::Gray;
        FLinearColor EndColor = FLinearColor::Red;
        FLinearColor CurrentColor = FMath::Lerp(StartColor, EndColor, ClampedProgress);
        SkillCombineCooldownBar->SetFillColorAndOpacity(CurrentColor);
    }
    
    if (SkillCombineCooldownText)
    {
        // Hard-code the display to show the key input (V for Combined Skill)
        SkillCombineCooldownText->SetText(FText::FromString(TEXT("V")));
    }
}

void UInGameUI::UpdateValues()
{
	if (!Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("UI Update: Player reference is null"));
		return;
	}

	// Update health
	if (HealthBar && CurrentHealthText && MaxHealthText)
	{
		HealthBar->SetPercent(static_cast<float>(Player->CurrentHealth) / Player->MaxHealth);
		CurrentHealthText->SetText(FText::AsNumber(Player->CurrentHealth));
		MaxHealthText->SetText(FText::AsNumber(Player->MaxHealth));
	}

	// Get the latest skill cooldown values
	float skill1Progress = Player->SkillCooldownProgress;
	float skill2Progress = Player->Skill2CooldownProgress;
    
	// Get the combination skill progress and log it for debugging
	float combinedSkillProgress = Player->GetSkillCombineCooldownProgress();
	UE_LOG(LogTemp, Verbose, TEXT("UI Update: Combined Skill Progress = %.2f"), combinedSkillProgress);
    
	// Update skill UI with these values
	UpdateSkillValues(skill1Progress, skill2Progress, combinedSkillProgress);
}

void UInGameUI::SetSetupRewindText(FString NewText)
{
	if (SetupRewindCooldownText) 
	{
		SetupRewindCooldownText->SetText(FText::FromString(NewText));
	}
}


