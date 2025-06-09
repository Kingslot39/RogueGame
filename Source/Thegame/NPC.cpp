// Fill out your copyright notice in the Description page of Project Settings.

#include "NPC.h"
#include "ThegameCharacter.h"
#include "Dialouge.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/BillboardComponent.h"
#include "GameFramework/PlayerController.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Blueprint/UserWidget.h"

// Sets default values
ANPC::ANPC()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    
    // Create and configure the interaction zone
    InteractionZone = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionZone"));
    InteractionZone->SetupAttachment(RootComponent);
    InteractionZone->SetSphereRadius(200.f); // Adjust as needed
    InteractionZone->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    InteractionZone->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    InteractionZone->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    InteractionZone->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

    // Create billboard component for easier selection in editor
    BillboardComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("BillboardComponent"));
    BillboardComponent->SetupAttachment(RootComponent);
    
    // Create widget component for interaction prompt
    InteractionPromptWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionPromptWidget"));
    InteractionPromptWidget->SetupAttachment(RootComponent);
    InteractionPromptWidget->SetWidgetSpace(EWidgetSpace::Screen);
    InteractionPromptWidget->SetDrawSize(FVector2D(200.0f, 50.0f));
    InteractionPromptWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f)); // Adjust to position above NPC's head
    InteractionPromptWidget->SetVisibility(false); // Hide by default

    bPlayerInRange = false;
    bShowingInteractionPrompt = false;
    bOffersWeaponSelection = false;

    
    
    // These settings are crucial for click detection to work properly
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
    GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Block);
    GetMesh()->SetGenerateOverlapEvents(true);
    
    // Initialize dialogue variables
    CurrentDialogueIndex = 0;
}

// Called when the game starts or when spawned
void ANPC::BeginPlay()
{
    Super::BeginPlay();
    
    // Bind overlap events
    InteractionZone->OnComponentBeginOverlap.AddDynamic(this, &ANPC::OnInteractionZoneBeginOverlap);
    InteractionZone->OnComponentEndOverlap.AddDynamic(this, &ANPC::OnInteractionZoneEndOverlap);

    // Bind click event with enhanced debugging
    GetMesh()->OnClicked.AddDynamic(this, &ANPC::OnClicked);
    UE_LOG(LogTemp, Warning, TEXT("NPC %s: Click events bound to mesh"), *GetName());
}

void ANPC::OnInteractionZoneBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor != this)
    {
        AThegameCharacter* PlayerCharacter = Cast<AThegameCharacter>(OtherActor);
        if (PlayerCharacter)
        {
            bPlayerInRange = true;
            
            // Check interaction limit
            if (MaxInteractionCount > 0 && CurrentInteractionCount >= MaxInteractionCount)
            {
                // Max interactions reached, do nothing
                return;
            }
            
            // Reset the weapon selection flag for this new interaction
            bWeaponSelectionAlreadyOpened = false;
            
            // Increment the counter
            CurrentInteractionCount++;
            
            // Disable player movement and start dialogue
            APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
            if (PlayerController)
            {
                PlayerCharacter->DisableInput(PlayerController);
            }
            
            // Open dialogue
            OpenDialogue();
        }
    }
}

void ANPC::OnInteractionZoneEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor && OtherActor != this)
    {
        AThegameCharacter* PlayerCharacter = Cast<AThegameCharacter>(OtherActor);
        if (PlayerCharacter)
        {
            bPlayerInRange = false;
            ShowInteractionPrompt(false);
            
            // Only close dialogue if not at max count
            if (MaxInteractionCount <= 0 || CurrentInteractionCount <= MaxInteractionCount)
            {
                // Close the dialogue if the player is leaving the interaction zone
                CloseDialogue();
            }
        }
    }
}

void ANPC::OnClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
    UE_LOG(LogTemp, Warning, TEXT("NPC %s clicked! Button: %s"), 
        *GetName(), *ButtonPressed.ToString());
    
    // Only proceed if player is in range
    if (bPlayerInRange)
    {
        UE_LOG(LogTemp, Warning, TEXT("Player in range, opening dialogue..."));
        OpenDialogue();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Player not in range for interaction."));
    }
}

void ANPC::Interact()
{
    UE_LOG(LogTemp, Warning, TEXT("NPC Interact called!"));
    
    // Hide the interaction prompt
    ShowInteractionPrompt(false);
    
    // Open dialogue
    OpenDialogue();
}

void ANPC::ShowInteractionPrompt(bool bShow)
{
    bShowingInteractionPrompt = bShow;
    
    // Update the widget visibility
    if (InteractionPromptWidget)
    {
        InteractionPromptWidget->SetVisibility(bShow);
    }
    
    UE_LOG(LogTemp, Display, TEXT("Interaction prompt %s"), bShow ? TEXT("shown") : TEXT("hidden"));
}

void ANPC::OpenDialogue()
{
    UE_LOG(LogTemp, Warning, TEXT("Opening dialogue. DialogueWidgetClass valid: %s"), 
           DialogueWidgetClass ? TEXT("YES") : TEXT("NO"));
    
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (PlayerController && DialogueWidgetClass)
    {
        DialogueWidgetInstance = CreateWidget<UDialouge>(PlayerController, DialogueWidgetClass);
        if (DialogueWidgetInstance)
        {
            DialogueWidgetInstance->AddToViewport();

            // Set input mode to UI only
            FInputModeUIOnly InputModeData;
            InputModeData.SetWidgetToFocus(DialogueWidgetInstance->TakeWidget());
            PlayerController->SetInputMode(InputModeData);
            PlayerController->bShowMouseCursor = true;

            // Optionally, pause the game
            PlayerController->SetPause(false);

            // Set the owning NPC in the widget
            DialogueWidgetInstance->SetOwningNPC(this);

            // Initialize dialogue index and show the first line
            CurrentDialogueIndex = 0;
            ShowCurrentDialogueLine();
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create DialogueWidgetInstance"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Player controller or DialogueWidgetClass is invalid"));
    }
}

void ANPC::ShowCurrentDialogueLine()
{
    if (DialogueWidgetInstance && DialogueLines.IsValidIndex(CurrentDialogueIndex))
    {
        const FDialogueLine& Line = DialogueLines[CurrentDialogueIndex];
        DialogueWidgetInstance->UpdateDialogueText(Line.Text);
    }
}

void ANPC::AdvanceDialogue()
{
    CurrentDialogueIndex++;
    if (DialogueLines.IsValidIndex(CurrentDialogueIndex))
    {
        ShowCurrentDialogueLine();
    }
    else
    {
        // End of dialogue
        CloseDialogue();
    }
}

void ANPC::CloseDialogue()
{
    if (DialogueWidgetInstance)
    {
        DialogueWidgetInstance->RemoveFromParent();
        DialogueWidgetInstance = nullptr;
    }

    // If this NPC offers weapon selection and it hasn't been opened yet for this interaction
    if (bOffersWeaponSelection && WeaponSelectionWidgetClass && !bWeaponSelectionAlreadyOpened)
    {
        // Mark that we've opened weapon selection for this interaction
        bWeaponSelectionAlreadyOpened = true;
        
        // Open the weapon selection UI
        OpenWeaponSelection();
        return; // Skip the rest of the method since we're keeping the player in UI mode
    }

    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (PlayerController)
    {
        // Set input mode back to Game and UI
        FInputModeGameAndUI InputModeData;
        PlayerController->SetInputMode(InputModeData);

        // Ensure the mouse cursor is visible
        PlayerController->bShowMouseCursor = true;

        // Unpause the game
        PlayerController->SetPause(false);

        // Re-enable player input if you disabled it
        if (AThegameCharacter* PlayerCharacter = Cast<AThegameCharacter>(PlayerController->GetPawn()))
        {
            PlayerCharacter->EnableInput(PlayerController);
        }
    }

    // Reset dialogue index for next interaction
    CurrentDialogueIndex = 0;
}

// Update the OpenWeaponSelection method in NPC.cpp to properly use the WeaponSelectionUI class

void ANPC::OpenWeaponSelection()
{
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (PlayerController && WeaponSelectionWidgetClass)
    {
        // Make sure player input is disabled during weapon selection
        AThegameCharacter* PlayerCharacter = Cast<AThegameCharacter>(PlayerController->GetPawn());
        if (PlayerCharacter)
        {
            // This ensures movement stops completely
            PlayerCharacter->DisableInput(PlayerController);
            
            // Also zero out velocity to prevent continuing movement
            if (PlayerCharacter->GetCharacterMovement())
            {
                PlayerCharacter->GetCharacterMovement()->StopMovementImmediately();
            }
        }

        // Create the weapon selection widget
        WeaponSelectionWidgetInstance = CreateWidget<UUserWidget>(PlayerController, WeaponSelectionWidgetClass);
        if (WeaponSelectionWidgetInstance)
        {
            WeaponSelectionWidgetInstance->AddToViewport();

            // Set input mode to UI only
            FInputModeUIOnly InputModeData;
            InputModeData.SetWidgetToFocus(WeaponSelectionWidgetInstance->TakeWidget());
            PlayerController->SetInputMode(InputModeData);
            PlayerController->bShowMouseCursor = true;

            // Cast to your specific WeaponSelectionUI class
            UWeaponSelectionUI* WeaponSelection = Cast<UWeaponSelectionUI>(WeaponSelectionWidgetInstance);
            if (WeaponSelection)
            {
                // Set up the weapon selection UI with the player character
                WeaponSelection->SetupWeaponSelection(PlayerCharacter);
            }
        }
    }
}
void ANPC::CloseWeaponSelection()
{
    // This would be a new method to add to your NPC class
    
    if (WeaponSelectionWidgetInstance)
    {
        UE_LOG(LogTemp, Display, TEXT("Closing weapon selection UI"));
        WeaponSelectionWidgetInstance->RemoveFromParent();
        WeaponSelectionWidgetInstance = nullptr;
    }
    
    // Re-enable player input
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (PlayerController)
    {
        // Set input mode back to Game and UI
        FInputModeGameAndUI InputModeData;
        PlayerController->SetInputMode(InputModeData);
        PlayerController->bShowMouseCursor = true;
        
        // Re-enable player input
        if (AThegameCharacter* PlayerCharacter = Cast<AThegameCharacter>(PlayerController->GetPawn()))
        {
            PlayerCharacter->EnableInput(PlayerController);
            UE_LOG(LogTemp, Display, TEXT("Player movement re-enabled after dialogue and weapon selection"));
        }
    }
}
// Called every frame
void ANPC::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ANPC::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}
bool ANPC::IsWeaponSelectionWidgetInstanceActive(UUserWidget* Widget) const
{
    return WeaponSelectionWidgetInstance == Widget;
}
