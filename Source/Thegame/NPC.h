// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DialogueStructs.h"
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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Function to advance the dialogue
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void AdvanceDialogue();
	// Function to close the dialogue
	void CloseDialogue();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
