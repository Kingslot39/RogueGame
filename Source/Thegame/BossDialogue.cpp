// BossDialogue.cpp
#include "BossDialogue.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "TimerManager.h"

void UBossDialogue::SetOwningBoss(ABossTest* NewBoss)
{
    OwningBoss = NewBoss;
}

void UBossDialogue::InitializeDialogue(ABossTest* InOwningBoss, const TArray<FDialogueLine>& InDialogueLines, float InDisplayTime)
{
    OwningBoss = InOwningBoss;
    DialogueLines = InDialogueLines;
    DialogueDisplayTime = InDisplayTime;
    CurrentDialogueLine = 0;
}

void UBossDialogue::UpdateDialogueContent(const FText& HeaderText, const FText& InDialogueContent)
{
    // Update header text if the widget exists
    if (HeaderTextBlock)
    {
        HeaderTextBlock->SetText(HeaderText);
    }
    
    // Update dialogue text using the existing method
    UpdateDialogueText(InDialogueContent);
}

void UBossDialogue::StartDialogue()
{
    if (!OwningBoss || DialogueLines.Num() == 0)
    {
        EndDialogue();
        return;
    }
    
    // Add dialogue to viewport
    AddToViewport();
    
    // Set the input mode to UI only
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (PlayerController)
    {
        AThegameCharacter* PlayerCharacter = Cast<AThegameCharacter>(PlayerController->GetPawn());
        if (PlayerCharacter && PlayerCharacter->InGameUI && PlayerCharacter->InGameUI->IsInViewport())
        {
            PlayerCharacter->InGameUI->RemoveFromParent();
        }
    }
    
    // Show the first dialogue line
    if (DialogueLines.IsValidIndex(CurrentDialogueLine))
    {
        UpdateDialogueContent(DialogueLines[CurrentDialogueLine].Header, DialogueLines[CurrentDialogueLine].Text);
    }
    
    // Hide the boss's health UI during dialogue
    if (OwningBoss && OwningBoss->TherionGameUI && OwningBoss->TherionGameUI->IsInViewport())
    {
        OwningBoss->TherionGameUI->RemoveFromParent();
    }
    
    // Disable movement for boss and player
    if (OwningBoss)
    {
        OwningBoss->GetCharacterMovement()->DisableMovement();
        
        if (OwningBoss->Target && OwningBoss->Target->GetCharacterMovement())
        {
            OwningBoss->Target->GetCharacterMovement()->DisableMovement();
        }
    }
}

void UBossDialogue::OnNextClicked()
{
    AdvanceDialogue();
}

void UBossDialogue::AdvanceDialogue()
{
    // Clear any existing timers
    GetWorld()->GetTimerManager().ClearTimer(DialogueTimerHandle);
    
    CurrentDialogueLine++;
    
    if (CurrentDialogueLine < DialogueLines.Num())
    {
        // Update both the header and dialogue text with the next line
        UpdateDialogueContent(DialogueLines[CurrentDialogueLine].Header, DialogueLines[CurrentDialogueLine].Text);
        
        // Set timer for next line if using automatic progression
        if (!NextButton)
        {
            GetWorld()->GetTimerManager().SetTimer(DialogueTimerHandle, this, &UBossDialogue::AdvanceDialogue, DialogueDisplayTime, false);
        }
    }
    else
    {
        // No more lines, end dialogue
        EndDialogue();
    }
}

void UBossDialogue::EndDialogue()
{
    // Clear any existing timers
    GetWorld()->GetTimerManager().ClearTimer(DialogueTimerHandle);
    
    // Remove dialogue from viewport
    RemoveFromParent();
    
    // Reset input mode
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (PlayerController)
    {
        AThegameCharacter* PlayerCharacter = Cast<AThegameCharacter>(PlayerController->GetPawn());
        if (PlayerCharacter && PlayerCharacter->InGameUI && !PlayerCharacter->InGameUI->IsInViewport())
        {
            PlayerCharacter->InGameUI->AddToViewport();
        }
    }
    
    // Notify the boss that dialogue is finished
    if (OwningBoss)
    {
        // Check if this is the first or second dialogue that's finishing
        if (OwningBoss->bSecondPhaseDialogueTriggered && !OwningBoss->bSecondPhaseDialogueFinished)
        {
            // Mark second phase dialogue as finished
            OwningBoss->bSecondPhaseDialogueFinished = true;
        }
        else
        {
            // Mark first dialogue as finished
            OwningBoss->bDialogueFinished = true;
        }
        
        // Re-enable player movement
        if (OwningBoss->Target && OwningBoss->Target->GetCharacterMovement())
        {
            OwningBoss->Target->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
        }
        
        // Only re-enable boss movement for the first dialogue
        if (!OwningBoss->bSecondPhaseDialogueTriggered)
        {
            OwningBoss->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
        }
        
        // Ensure the health UI is visible after the first dialogue
        if (!OwningBoss->bSecondPhaseDialogueTriggered && OwningBoss->TherionGameUI && !OwningBoss->TherionGameUI->IsInViewport())
        {
            OwningBoss->TherionGameUI->AddToViewport();
        }
    }
}