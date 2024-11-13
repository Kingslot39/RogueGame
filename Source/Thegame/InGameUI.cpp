// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameUI.h"
#include "ThegameCharacter.h"
#include "Components/ProgressBar.h"

void UInGameUI::UpdateValues()
{
	if (Player && HealthBar && CurrentHealthText && MaxHealthText)  // Ensure all pointers are valid
	{
		// Update health bar and text values
		HealthBar->SetPercent(static_cast<float>(Player->CurrentHealth) / static_cast<float>(Player->MaxHealth));
		CurrentHealthText->SetText(FText::FromString(FString::FromInt(static_cast<int32>(Player->CurrentHealth))));
		MaxHealthText->SetText(FText::FromString(FString::FromInt(static_cast<int32>(Player->MaxHealth))));
        
		// Update skill cooldowns
		UpdateSkillCooldown(Player->SkillCooldownProgress, Skill1CooldownBar);
		UpdateSkill2Cooldown(Player->Skill2CooldownProgress, Skill2CooldownBar);
		// Detect death
		if (Player->CurrentHealth <= 0.0f)
		{
			// Call a BlueprintImplementableEvent to display the death screen
			OnPlayerDeath();
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid UI or Player reference in UInGameUI::UpdateValues"));
	}
}
void UInGameUI::UpdateSkillCooldown(float ProgressValue, UProgressBar* SkillCooldownBar)
{
	if (SkillCooldownBar)
	{
		SkillCooldownBar->SetPercent(ProgressValue);

		// Set color for the progress bar fill
		FLinearColor StartColor = FLinearColor::Gray;  // Color when cooldown starts
		FLinearColor EndColor = FLinearColor::Blue;    // Color when cooldown ends
		FLinearColor CurrentColor = FMath::Lerp(StartColor, EndColor, ProgressValue);
		SkillCooldownBar->SetFillColorAndOpacity(CurrentColor);

		if (Skill1CooldownText)  // Check if Skill1CooldownText is valid
		{
			FLinearColor TextColor = FLinearColor::Red;
			float Opacity = FMath::Lerp(0.3f, 1.0f, ProgressValue);
			TextColor.A = Opacity;
			Skill1CooldownText->SetColorAndOpacity(FSlateColor(TextColor));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Skill1CooldownText is null in UpdateSkillCooldown"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillCooldownBar is null in UpdateSkillCooldown"));
	}
}
void UInGameUI::UpdateSkill2Cooldown(float ProgressValue, UProgressBar* InSkill2CooldownBar)
{
	if (InSkill2CooldownBar)
	{
		InSkill2CooldownBar->SetPercent(ProgressValue);

		// Set color for the progress bar fill
		FLinearColor StartColor = FLinearColor::Gray;  // Color when cooldown starts
		FLinearColor EndColor = FLinearColor::Green;    // Color when cooldown ends
		FLinearColor CurrentColor = FMath::Lerp(StartColor, EndColor, ProgressValue);
		InSkill2CooldownBar->SetFillColorAndOpacity(CurrentColor);

		if (Skill2CooldownText)  // Check if Skill2CooldownText is valid
		{
			FLinearColor TextColor = FLinearColor::Yellow;
			float Opacity = FMath::Lerp(0.3f, 1.0f, ProgressValue);
			TextColor.A = Opacity;
			Skill2CooldownText->SetColorAndOpacity(FSlateColor(TextColor));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Skill2CooldownText is null in UpdateSkill2Cooldown"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InSkill2CooldownBar is null in UpdateSkill2Cooldown"));
	}
}