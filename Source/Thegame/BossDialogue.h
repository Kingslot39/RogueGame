// BossDialogue.h
#pragma once

#include "CoreMinimal.h"
#include "Dialouge.h"
#include "BossTest.h"
#include "DialogueStructs.h"
#include "Components/TextBlock.h"
#include "BossDialogue.generated.h"

UCLASS()
class THEGAME_API UBossDialogue : public UDialouge
{
	GENERATED_BODY()

public:
	// Initialize the dialogue with all needed data
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void InitializeDialogue(ABossTest* InOwningBoss, const TArray<FDialogueLine>& InDialogueLines, float InDisplayTime = 3.0f);
    
	// Start the dialogue sequence
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void StartDialogue();
    
	// Handle advancing to the next dialogue line
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void AdvanceDialogue();
    
	// End the dialogue sequence
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void EndDialogue();
    
	
	virtual void OnNextClicked();
    
	// Set the owning boss
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void SetOwningBoss(ABossTest* NewBoss);
	// Add method to update both header and text
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void UpdateDialogueContent(const FText& HeaderText, const FText& InDialogueContent);
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* HeaderTextBlock;
private:
	// Reference to the owning boss
	UPROPERTY()
	ABossTest* OwningBoss;
    
	// The dialogue lines to display
	UPROPERTY()
	TArray<FDialogueLine> DialogueLines;
    
	// Current dialogue line index
	UPROPERTY()
	int32 CurrentDialogueLine = 0;
    
	// Time to display each dialogue line
	UPROPERTY()
	float DialogueDisplayTime = 3.0f;
    
	// Timer handle for automatic dialogue progression
	FTimerHandle DialogueTimerHandle;
};