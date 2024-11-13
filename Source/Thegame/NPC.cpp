// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC.h"
#include "ThegameCharacter.h"
#include "Dialouge.h"
#include "Components/SphereComponent.h"
#include "GameFramework/PlayerController.h"
#include "TheGameCharacter.h" // Replace with your player character header

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

	bPlayerInRange = false;

	// Enable click events on the mesh
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);

	CurrentDialogueIndex = 0;
}

// Called when the game starts or when spawned
void ANPC::BeginPlay()
{
	Super::BeginPlay();
	// Bind overlap events
	InteractionZone->OnComponentBeginOverlap.AddDynamic(this, &ANPC::OnInteractionZoneBeginOverlap);
	InteractionZone->OnComponentEndOverlap.AddDynamic(this, &ANPC::OnInteractionZoneEndOverlap);

	// Bind click event
	GetMesh()->OnClicked.AddDynamic(this, &ANPC::OnClicked);
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
			// Close the dialogue if the player is leaving the interaction zone
			CloseDialogue();
		}
	}
}

void ANPC::OnClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	OpenDialogue();
}

void ANPC::OpenDialogue()
{
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

