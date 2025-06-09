// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "TherionGameUI.generated.h"

/**
 * 
 */
class ABossTest;
UCLASS()
class THEGAME_API UTherionGameUI : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;
	UPROPERTY(EditAnywhere)
	ABossTest* Target;
	void UpdateValue();
	
};
