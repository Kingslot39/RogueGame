// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyHealthWidget.generated.h"

/**
 * 
 */
UCLASS()
class THEGAME_API UEnemyHealthWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	// Progress Bar for Health
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	// Function to update health bar
	UFUNCTION(BlueprintCallable, Category = "Health")
	void UpdateHealthBar(float HealthPercentage);
};
