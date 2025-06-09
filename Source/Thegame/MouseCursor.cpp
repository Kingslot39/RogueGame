#include "MouseCursor.h"
#include "GameFramework/Pawn.h"

AMouseCursorController::AMouseCursorController()
{
	bShowMouseCursor = false;  // Hide the cursor by default
	bEnableClickEvents = false;
	bEnableMouseOverEvents = false;
}

void AMouseCursorController::BeginPlay()
{
	Super::BeginPlay();
	//Ensure MouseMode.
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
}
void AMouseCursorController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	RotateToMouseCursor();
}

void AMouseCursorController::RotateToMouseCursor()
{
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit); // Visibility channel.

	if (Hit.bBlockingHit)
	{
		APawn* ControlledPawn = GetPawn();
		if (ControlledPawn)
		{
			FVector WorldDirection = (Hit.ImpactPoint - ControlledPawn->GetActorLocation()).GetSafeNormal();
			FRotator NewRotation = WorldDirection.Rotation();
			NewRotation.Pitch = 0.0f;  // Keep horizontal
			NewRotation.Roll = 0.0f;

			ControlledPawn->SetActorRotation(NewRotation);
		}
	}
}
