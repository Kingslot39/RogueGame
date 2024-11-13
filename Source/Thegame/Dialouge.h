// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Dialouge.generated.h"

/**
 * 
 */
UCLASS()
class THEGAME_API UDialouge : public UUserWidget
{
	GENERATED_BODY()
public:
	// Function to update the dialogue text
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void UpdateDialogueText(const FText& NewText);

	// Function to set the owning NPC
	void SetOwningNPC(class ANPC* NewNPC);
protected:
	// Reference to the Dialogue Text Block
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DialogueText;

	// Reference to the Next Button
	UPROPERTY(meta = (BindWidget))
	class UButton* NextButton;

	// Reference to the owning NPC
	UPROPERTY()
	class ANPC* OwningNPC;

	virtual void NativeConstruct() override;

	// Function to handle Next Button click
	UFUNCTION()
	void OnNextClicked();
};
