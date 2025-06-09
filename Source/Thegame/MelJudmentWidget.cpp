// Fill out your copyright notice in the Description page of Project Settings.


#include "MelJudmentWidget.h"

#include "MelBoss.h"

void UMelJudmentWidget::UpdateValue()
{
	if (!HealthBar || !Target)
	{
		// Either HealthBar or Target is null, can't update
		UE_LOG(LogTemp, Warning, TEXT("Cannot update health bar: %s is null"), 
			!HealthBar ? TEXT("HealthBar") : TEXT("Target"));
		return;
	}
    
	HealthBar->SetPercent((float)Target->CurrentHealth/Target->MaxHealth);
}
