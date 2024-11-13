// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ThegameCharacter.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "MouseCursor.generated.h"

/**
 * 
 */
UCLASS()
class THEGAME_API AMouseCursor : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
public:
	AMouseCursor();
	virtual  void SetupInputComponent() override;

	void OnMouseX(float AxisValue);
	void OnMouseY(float AxisValue);



	
	
	
	
	
};

