// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "MelJudmentWidget.generated.h"

/**
 * 
 */
class AMelBoss;
UCLASS()
class THEGAME_API UMelJudmentWidget : public UUserWidget
{
	GENERATED_BODY()
  public:
  UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;
 	UPROPERTY(EditAnywhere)
 	AMelBoss* Target;
 	void UpdateValue();
};
