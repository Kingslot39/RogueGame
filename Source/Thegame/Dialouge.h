// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h" 
#include "Components/Button.h"    
#include "Dialouge.generated.h"

/**
 * 
 */
UCLASS()
class THEGAME_API UDialouge : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* DialogueText;
    
	UPROPERTY(meta = (BindWidget))
	UButton* NextButton;
    
	// Base class virtual function to update dialogue text
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	virtual void UpdateDialogueText(const FText& NewText);
    
	// NPC reference - only used by base dialogue class
	UPROPERTY()
	class ANPC* OwningNPC;
    
	// Set the owning NPC
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void SetOwningNPC(ANPC* NewNPC);
    
	// Base function for all dialogue widgets
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	virtual void OnNextClicked();
    
protected:
	virtual void NativeConstruct() override;
};
