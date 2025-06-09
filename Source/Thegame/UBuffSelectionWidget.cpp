// Fill out your copyright notice in the Description page of Project Settings.


#include "UBuffSelectionWidget.h"
#include "ThegameCharacter.h"
#include "InGameUI.h"
#include "RotatingShield.h"
#include "Components/Button.h"


void UUBuffSelectionWidget::InitializeBuffOptions()
{
}
void UUBuffSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();
	AThegameCharacter* Player = Cast<AThegameCharacter>(GetOwningPlayerPawn());
	if(Player)
	{
		if(Player->bHasReroll)
		{
			RerollButton->SetVisibility(ESlateVisibility::Visible);
			RerollButton->OnClicked.AddDynamic(this, &UUBuffSelectionWidget::OnRerollClicked);
		}
		else
		{
			RerollButton->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	// Shuffle the buff options when the widget is constructed
	ShuffleAndDisplayBuffOptions();
    
	// Bind button events
	BindButtonEvents();
}

void UUBuffSelectionWidget::OnFadeAnimationCompleted()
{
	UE_LOG(LogTemp, Warning, TEXT("Fade Animation completed - enabling buttons"));
    
	// IMPORTANT: Set the blocking image to Collapsed, not just Hidden
	if (BlockingImage) 
	{
		BlockingImage->SetVisibility(ESlateVisibility::Collapsed);
	}
    
	// Make sure buttons are enabled
	if (BuffButton1) BuffButton1->SetIsEnabled(true);
	if (BuffButton2) BuffButton2->SetIsEnabled(true);
	if (BuffButton3) BuffButton3->SetIsEnabled(true);
	if (RerollButton && RerollButton->IsVisible()) 
	{
		RerollButton->SetIsEnabled(true);
	}
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
				// Generate a random speed increase between 10% and 50%
				float RandomSpeedIncrease = FMath::RandRange(0.1f, 0.5f);
    
				// Apply the speed increase to the player's multiplier
				Player->SpeedMultiplier += RandomSpeedIncrease;
    
				// Calculate the new speed for logging
				float NewSpeed = Player->BaseSpeed * Player->SpeedMultiplier;
    
				// Log the speed change
				UE_LOG(LogTemp, Warning, TEXT("Speed increased by %.1f%%. New multiplier: %.2f, New speed: %.2f"),
					RandomSpeedIncrease * 100.0f, Player->SpeedMultiplier, NewSpeed);
    
				// Give immediate feedback to the player
				FString SpeedMessage = FString::Printf(TEXT("Speed increased by %.0f%%!"), RandomSpeedIncrease * 100.0f);
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, SpeedMessage);
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
			// Rotating Shield buff
			{
				Player->ActivateRotatingShield();
				
				// Give immediate feedback to the player
				FString ShieldMessage = TEXT("Rotating Shield activated! Damages and stuns enemies!");
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, ShieldMessage);
				
				UE_LOG(LogTemp, Warning, TEXT("Rotating Shield buff activated!"));
			}
			break;
		case 4: // Reroll buff
			Player->bHasReroll = true;
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
	// Create an array with buff indices (0 to 2)
	TArray<int32> BuffIndices = {0, 1, 2};
	
	// Get player reference safely
	AThegameCharacter* Player = Cast<AThegameCharacter>(GetOwningPlayerPawn());
	
	// Add rotating shield buff only if player doesn't already have it
	if(Player && !Player->bHasRotatingShield)
	{
		BuffIndices.Add(3); // Index 3 = Rotating Shield
	}
	
	// Add reroll buff only if player exists AND hasn't unlocked it yet
	if(Player && !Player->bHasReroll)
	{
		BuffIndices.Add(4); // Index 4 = Reroll buff
	}
	
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
			FontInfo.Size = 24;  // Adjust the size value to make the text smaller

			// Apply the updated font to the TextBlock
			ButtonText->SetFont(FontInfo); // Use SetFont() instead of accessing Font directly
			// Set the tooltip text to empty
			BuffButton->SetToolTipText(FText::GetEmpty());
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
				ButtonText->SetText(FText::FromString("Rotating Shield"));
				break;
			case 4:
				ButtonText->SetText(FText::FromString("Reroll Unlock"));
				break;
			default:
				ButtonText->SetText(FText::FromString("Unknown Buff"));
				break;
			}
		}
	}
}

void UUBuffSelectionWidget::OnRerollClicked()
{
	ShuffleAndDisplayBuffOptions();
}

void UUBuffSelectionWidget::UpdateRerollButtonVisibility()
{
}
