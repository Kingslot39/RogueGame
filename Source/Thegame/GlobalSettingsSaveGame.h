// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GlobalSettingsSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class THEGAME_API UGlobalSettingsSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY()
	float GlobalExposure = 1.5f; // Default value
};
