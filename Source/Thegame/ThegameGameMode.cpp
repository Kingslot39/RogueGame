// Copyright Epic Games, Inc. All Rights Reserved.

#include "ThegameGameMode.h"
#include "UBuffSelectionWidget.h"
#include "MouseCursor.h"
#include "ThegameCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"


AThegameGameMode::AThegameGameMode()
{

	PlayerControllerClass = AMouseCursor::StaticClass();
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	EnemyKillCount = 0;
	KillsRequiredForBuff = 10;
	
}
void AThegameGameMode::HandleEnemyDeath()
{
	EnemyKillCount++;
    
	// Log the current kill count
	UE_LOG(LogTemp, Warning, TEXT("EnemyKillCount: %d"), EnemyKillCount);

	if (EnemyKillCount >= KillsRequiredForBuff)
	{
		EnemyKillCount = 0;  // Reset the count
		UE_LOG(LogTemp, Warning, TEXT("Buff Popup!"));  // Debug log to confirm it reaches the required kill count

		ShowBuffSelection();  // This function is implemented in Blueprint
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Kills needed for Buff: %d"), KillsRequiredForBuff - EnemyKillCount);
	}
}

void AThegameGameMode::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetFirstPlayerController()->SetInputMode(FInputModeGameOnly());
    GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);


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