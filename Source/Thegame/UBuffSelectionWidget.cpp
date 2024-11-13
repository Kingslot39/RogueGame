// Fill out your copyright notice in the Description page of Project Settings.


#include "UBuffSelectionWidget.h"
#include "ThegameCharacter.h"
#include "InGameUI.h"
#include "ThegameGameMode.h"
#include "Components/Button.h"
#include "GameFramework/CharacterMovementComponent.h"

void UUBuffSelectionWidget::InitializeBuffOptions()
{
}
void UUBuffSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();
    
	// Shuffle the buff options when the widget is constructed
	ShuffleAndDisplayBuffOptions();
    
	// Bind button events
	BindButtonEvents();
}
void UUBuffSelectionWidget::OnBuffSelected(int32 BuffIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("Buff %d selected!"), BuffIndex);

	// Assuming you have a reference to the player character
	AThegameCharacter* Player = Cast<AThegameCharacter>(GetOwningPlayerPawn());
	if (Player)
	{
		switch (BuffIndex)
		{
		case 0:  // Increase speed buff
			{
				// Get the character movement component
				UCharacterMovementComponent* CharacterMovement = Player->GetCharacterMovement();
				if (CharacterMovement)
				{
					// Check if current speed is valid
					if (CharacterMovement->MaxWalkSpeed <= 0.0f)
					{
						UE_LOG(LogTemp, Warning, TEXT("MaxWalkSpeed was invalid (<=0). Resetting to default value."));
						CharacterMovement->MaxWalkSpeed = 700.0f;  // Reset to default value
					}
					// Store the current speed
					float CurrentSpeed = CharacterMovement->MaxWalkSpeed;
					// Generate a random speed increase between 100 and 300
					float RandomSpeedIncrease = FMath::RandRange(10000.0f, 30000.0f);
					// Update the MaxWalkSpeed
					CharacterMovement->MaxWalkSpeed += RandomSpeedIncrease;
					// Log the speed change for debugging
					UE_LOG(LogTemp, Warning, TEXT("Speed increased from %f to %f"), CurrentSpeed, CharacterMovement->MaxWalkSpeed);
				}
			}
			break;
			
		case 1:
			{
				// Generate a random value between 1 and 20
				int32 RandomHealthIncrease = FMath::RandRange(1, 20);

				// Increase Max Health and Current Health
				Player->MaxHealth += RandomHealthIncrease;
				Player->CurrentHealth += RandomHealthIncrease;

				// Update the UI to reflect new health values
				Player->InGameUI->UpdateValues(); // Refresh the UI to reflect new health values

				UE_LOG(LogTemp, Warning, TEXT("Health increased by %d. New Max Health: %f, Current Health: %f"),
					RandomHealthIncrease, Player->MaxHealth, Player->CurrentHealth);
			}
			break;

		case 2:
			// Buff case 2 logic
			{
				// Reduce skill cooldown time by 1 second, ensuring it doesn't go below zero
				Player->BaseSkillCooldownTime = FMath::Max(0.0f, Player->BaseSkillCooldownTime - 1.0f);

				// Increase skill damage
				Player->SkillDamage += 10.0f; // Increase damage by 10 units

				// Log the changes for debugging
				UE_LOG(LogTemp, Warning, TEXT("Buff 2 applied! Skill cooldown reduced to %f seconds, Skill damage increased to %f"),
					Player->BaseSkillCooldownTime, Player->SkillDamage);
			}
			break;
		case 3:
			// Buff case 3 logic (placeholder)
				UE_LOG(LogTemp, Warning, TEXT("Buff 3 selected! No specific logic implemented yet."));
			break;

		default:
			UE_LOG(LogTemp, Warning, TEXT("Invalid buff index selected!"));
			break;
		}
	}

	// Unpause the game when a buff is selected
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		PlayerController->SetPause(false);  // Unpause the game

		// Set the input mode to Game and UI to keep both game input and mouse cursor active
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PlayerController->SetInputMode(InputMode);

		PlayerController->bShowMouseCursor = true;  // Keep the mouse cursor visible
	}

	// Close the widget after selecting a buff
	RemoveFromParent(); 
}
void UUBuffSelectionWidget::ShuffleAndDisplayBuffOptions()
{
	// Create an array with buff indices (0 to 3)
	TArray<int32> BuffIndices = {0, 1, 2, 3};

	// Shuffle the array to get a random order
	for (int32 i = BuffIndices.Num() - 1; i > 0; i--)
	{
		int32 RandomIndex = FMath::RandRange(0, i);
		BuffIndices.Swap(i, RandomIndex);
	}

	// Select the first 3 shuffled indices
	SelectedBuffIndices = {BuffIndices[0], BuffIndices[1], BuffIndices[2]};

	// Update UI Buttons with the selected buffs
	if (BuffButton1 && BuffButton2 && BuffButton3)
	{
		// Make buttons visible and enabled
		BuffButton1->SetIsEnabled(true);
		BuffButton2->SetIsEnabled(true);
		BuffButton3->SetIsEnabled(true);

		// Update button text or description to match the selected buffs
		UpdateBuffButtonText(BuffButton1, SelectedBuffIndices[0]);
		UpdateBuffButtonText(BuffButton2, SelectedBuffIndices[1]);
		UpdateBuffButtonText(BuffButton3, SelectedBuffIndices[2]);
	}
}

void UUBuffSelectionWidget::BindButtonEvents()
{
	if (BuffButton1)
	{
		BuffButton1->OnClicked.AddDynamic(this, &UUBuffSelectionWidget::OnBuffButtonClicked1);
	}

	if (BuffButton2)
	{
		BuffButton2->OnClicked.AddDynamic(this, &UUBuffSelectionWidget::OnBuffButtonClicked2);
	}

	if (BuffButton3)
	{
		BuffButton3->OnClicked.AddDynamic(this, &UUBuffSelectionWidget::OnBuffButtonClicked3);
	}
}

void UUBuffSelectionWidget::OnBuffButtonClicked1()
{
	if (SelectedBuffIndices.Num() > 0)
	{
		OnBuffSelected(SelectedBuffIndices[0]);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SelectedBuffIndices array is empty!"));
	}
}

void UUBuffSelectionWidget::OnBuffButtonClicked2()
{
	if (SelectedBuffIndices.Num() > 1)
	{
		OnBuffSelected(SelectedBuffIndices[1]);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SelectedBuffIndices array is empty or not initialized correctly!"));
	}
}

void UUBuffSelectionWidget::OnBuffButtonClicked3()
{
	if (SelectedBuffIndices.Num() > 2)
	{
		OnBuffSelected(SelectedBuffIndices[2]);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SelectedBuffIndices array is empty or not initialized correctly!"));
	}
}
void UUBuffSelectionWidget::UpdateBuffButtonText(UButton* BuffButton, int32 BuffIndex)
{
	if (BuffButton)
	{
		UTextBlock* ButtonText = Cast<UTextBlock>(BuffButton->GetChildAt(0));
		if (ButtonText)
		{
			// Create a FSlateFontInfo and modify the font size to make it smaller
			FSlateFontInfo FontInfo = ButtonText->GetFont(); // Use GetFont() instead of accessing Font directly
			FontInfo.Size = 12;  // Adjust the size value to make the text smaller

			// Apply the updated font to the TextBlock
			ButtonText->SetFont(FontInfo); // Use SetFont() instead of accessing Font directly

			// Set the text content based on the BuffIndex
			switch (BuffIndex)
			{
			case 0:
				ButtonText->SetText(FText::FromString("Speed Increase"));
				break;

			case 1:
				ButtonText->SetText(FText::FromString("Health Increase"));
				break;

			case 2:
				ButtonText->SetText(FText::FromString("SkillArrow Buff"));
				break;

			case 3:
				ButtonText->SetText(FText::FromString("Example Buff 3"));
				break;

			default:
				ButtonText->SetText(FText::FromString("Unknown Buff"));
				break;
			}
		}
	}
}
