// Fill out your copyright notice in the Description page of Project Settings.


#include "MouseCursor.h"
AMouseCursor::AMouseCursor()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}

void AMouseCursor::BeginPlay()
{
	
	Super::BeginPlay();
    
	// set up the cursor widget
	
}

void AMouseCursor::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis("MouseX",this, &AMouseCursor::OnMouseX);
	InputComponent->BindAxis("MouseY", this, &AMouseCursor::OnMouseY);
}

void AMouseCursor::OnMouseX(float AxisValue)
{
	if(AxisValue!= 0)
	{
		FRotator NewRotator = GetControlRotation();

		NewRotator.Yaw += AxisValue * 0.3;//MouseSensitivity;

		SetControlRotation(NewRotator);
	}
}

void AMouseCursor::OnMouseY(float AxisValue)
{
	if(AxisValue!= 0)
	{
		FRotator NewRotator = GetControlRotation();

		NewRotator.Pitch += AxisValue * 1;//MouseSensitivity;

		NewRotator.Pitch = FMath::Clamp(NewRotator.Pitch, -80, 80);

		SetControlRotation(NewRotator);
	}
}

