// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DialogueStructs.h"
#include "WeaponSelection.h"
#include "NPC.generated.h"

UCLASS()
class THEGAME_API ANPC : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANPC();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* InteractionZone;
	
	// Billboard component to make it easier to select NPCs in the editor
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UBillboardComponent* BillboardComponent;
	
	// Widget component for interaction prompt
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UWidgetComponent* InteractionPromptWidget;

	// Functions
	UFUNCTION()
	void OnInteractionZoneBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
									   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
									   bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnInteractionZoneEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
									 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);

	// Dialogue handling
	UFUNCTION()
	void OpenDialogue();

	// Variables
	bool bPlayerInRange;
	
	// Whether an interaction prompt is currently displayed
	bool bShowingInteractionPrompt;

	// Dialogue widget class reference
	UPROPERTY(EditDefaultsOnly, Category = "Dialogue")
	TSubclassOf<class UUserWidget> DialogueWidgetClass;

	// Array to hold the dialogue lines
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	TArray<FDialogueLine> DialogueLines;

	// Index to keep track of the current dialogue line
	int32 CurrentDialogueIndex;

	// Reference to the dialogue widget instance
	UPROPERTY()
	class UDialouge* DialogueWidgetInstance;
	
	// Functions to handle dialogue progression
	void ShowCurrentDialogueLine();
	
	// Function to show/hide interaction prompt
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void ShowInteractionPrompt(bool bShow);
	
	// Whether this NPC offers weapon selection after dialogue
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Selection")
	bool bOffersWeaponSelection;
	
	// The WeaponSelectionUI widget class to instantiate
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Selection", meta = (EditCondition = "bOffersWeaponSelection"))
	TSubclassOf<class UUserWidget> WeaponSelectionWidgetClass;
	
	// The current weapon selection widget instance
	UPROPERTY()
	class UUserWidget* WeaponSelectionWidgetInstance;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Function to advance the dialogue
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void AdvanceDialogue();
	
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void CloseWeaponSelection();
	// Function to close the dialogue
	void CloseDialogue();
	
	// Function to directly interact with the NPC (can be called from player)
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void Interact();
	
	// Open the weapon selection UI
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void OpenWeaponSelection();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	bool IsWeaponSelectionWidgetInstanceActive(UUserWidget* Widget) const;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Maximum number of times the player can interact with this NPC
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	int32 MaxInteractionCount = 1;

	// Current number of times the player has interacted with this NPC
	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	int32 CurrentInteractionCount = 0;

	UPROPERTY()
	bool bWeaponSelectionAlreadyOpened = false;
};