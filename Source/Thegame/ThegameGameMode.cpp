// Copyright Epic Games, Inc. All Rights Reserved.

#include "ThegameGameMode.h"


#include "UBuffSelectionWidget.h"
#include "MyGameInstance.h"

#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

AThegameGameMode::AThegameGameMode()
{
	// Use the default PlayerController instead of AMouseCursorController
	// PlayerControllerClass = AMouseCursorController::StaticClass();
	
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	EnemyKillCount = 0;
	KillsRequiredForBuff = 5;
}

void AThegameGameMode::HandleEnemyDeath()
{
	EnemyKillCount++;
	if (EnemyKillCount >= KillsRequiredForBuff)
	{
		UE_LOG(LogTemp, Display, TEXT("Reached kill threshold! Opening buff selection"));
        
		// Show buff selection UI
		ShowBuffSelection();
        
		// Reset kill counter
		EnemyKillCount = 0;
	}
}

void AThegameGameMode::BeginPlay()
{
	Super::BeginPlay();
	// Get a reference to the custom GameInstance once.
	UMyGameInstance* GI = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (GI)
	{
		// Reset stage clear flag
		GI->bStageClear = false;
		UE_LOG(LogTemp, Log, TEXT("GameMode BeginPlay: Reset bStageClear to false"));
	}

	// Set input mode and show mouse cursor
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		// Use GameAndUI mode to allow both WASD control and mouse cursor
		FInputModeGameAndUI InputMode;
		InputMode.SetHideCursorDuringCapture(false);
		PC->SetInputMode(InputMode);
		PC->SetShowMouseCursor(true);
	}

	// Reposition player if needed
	if (GI && GI->bShouldApplySavedTransform)
	{
		if (PC)
		{
			APawn* Pawn = PC->GetPawn();
			if (Pawn)
			{
				Pawn->SetActorTransform(GI->SavedPlayerTransform);
				UE_LOG(LogTemp, Log, TEXT("Player repositioned after level load to: %s"),
					*GI->SavedPlayerTransform.GetLocation().ToString());
                
				// Reset the flag so this only happens once
				GI->bShouldApplySavedTransform = false;
			}
		}
	}
}

void AThegameGameMode::ShowBuffSelection()
{	
	if (BuffSelectionWidgetClass)
	{
		UUserWidget* WidgetBase = CreateWidget<UUserWidget>(GetWorld(), BuffSelectionWidgetClass);

		if (WidgetBase)
		{
			// Cast to UUBuffSelectionWidget to access specific functionality
			UUBuffSelectionWidget* BuffSelectionWidget = Cast<UUBuffSelectionWidget>(WidgetBase);

			if (BuffSelectionWidget)
			{
				BuffSelectionWidget->AddToViewport();

				// Pause the game
				APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
				if (PlayerController)
				{
					PlayerController->SetPause(true);  // Pauses the game
					FInputModeUIOnly InputMode;
					InputMode.SetWidgetToFocus(BuffSelectionWidget->TakeWidget());
					PlayerController->SetInputMode(InputMode);
					PlayerController->bShowMouseCursor = true;  // Show the mouse cursor for UI interaction
				}

				BuffSelectionWidget->InitializeBuffOptions();  // Initialize options if needed
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to cast WidgetBase to UUBuffSelectionWidget."));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to create BuffSelectionWidget."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BuffSelectionWidgetClass is not set in GameMode."));
	}
}