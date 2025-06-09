// Fill out your copyright notice in the Description page of Project Settings.


#include "Dialouge.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "NPC.h"

void UDialouge::UpdateDialogueText(const FText& NewText)
{
	if (DialogueText)
	{
		DialogueText->SetText(NewText);
	}
}

void UDialouge::SetOwningNPC(ANPC* NewNPC)
{
	OwningNPC = NewNPC;
}

void UDialouge::NativeConstruct()
{
	Super::NativeConstruct();
	if (NextButton)
	{
		NextButton->OnClicked.Clear();
		NextButton->OnClicked.AddDynamic(this, &UDialouge::OnNextClicked);
	}
}
	
void UDialouge::OnNextClicked()
{
	if (OwningNPC)
	{
		OwningNPC->AdvanceDialogue();
	}
}
