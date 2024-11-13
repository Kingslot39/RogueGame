// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ThegameGameMode.generated.h"

class UBuffSelectionWidget;
UCLASS(minimalapi)
class AThegameGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AThegameGameMode();
	UPROPERTY(BlueprintReadWrite, Category = "Roguelike")
	int32 EnemyKillCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Roguelike")
	int32 KillsRequiredForBuff;

	UFUNCTION(BlueprintCallable, Category = "Roguelike")
	void HandleEnemyDeath();
    
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowBuffSelection();

	// Reference to the buff selection widget class
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> BuffSelectionWidgetClass;
	
	virtual void BeginPlay() override;

};



