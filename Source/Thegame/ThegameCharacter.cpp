// Copyright Epic Games, Inc. All Rights Reserved.

#include "ThegameCharacter.h"
#include "Enemy.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AThegameCharacter




AThegameCharacter::AThegameCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	CanDashing = true;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 700.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	 CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
	MaxHealth = 100.0f; // Set max health
	CurrentHealth = MaxHealth; // Initialize current health to max health

	SwordMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sword Mesh"));
	SwordMesh->SetupAttachment(GetMesh(), FName("Sword_RightHand"));
	//SwordDamageArea = CreateDefaultSubobject<UBoxComponent>(TEXT("Damage Area"));
	//SwordDamageArea->SetupAttachment(SwordMesh);

	// Initialize base skill cooldown time and skill damage
	BaseSkillCooldownTime = 3.0f; // Default cooldown time
	SkillDamage = 50.0f;          // Default skill damage

	Skill2CooldownTime = BaseSkill2CooldownTime;

	//Pause
	bIsPaused = false;

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		PlayerController->bShowMouseCursor = true;
		PlayerController->bEnableClickEvents = true;
		PlayerController->bEnableMouseOverEvents = true;
	}
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);

}



void AThegameCharacter::Setup()
{
	if(bAlreadySetup == false)
	{
   	
		FVector Location = GetActorLocation() - FVector(0,0,100);
		SpawnTeleport = (GetWorld()->SpawnActor(LandmarkClass, &Location));
		bAlreadySetup = true;
		bCanRewind = true;
	}

}


void AThegameCharacter::Rewind()
{
	if(bCanRewind)
	{
		FVector TeleportLocation = SpawnTeleport->GetActorLocation();
		SetActorLocation(TeleportLocation);
		SpawnTeleport->Destroy();
		bCanRewind = false;
		bAlreadySetup = false;
		
		bIsSkill2Ready = true;
		bIsSkillReady = true;
		bIsSkillComReady = true;
	}

}

void AThegameCharacter::SkillActivate()
{
	if (!bIsSkillReady)
	{
		UE_LOG(LogTemp, Warning, TEXT("Skill is on cooldown!"));
		return;
	}

	// Set skill on cooldown
	bIsSkillReady = false;
	SkillCooldownProgress = 0.0f;
	SkillCooldownTime = BaseSkillCooldownTime; // Use the base cooldown time

	// Start the cooldown timer
	GetWorld()->GetTimerManager().SetTimer(SkillCooldownTimerHandle, this, &AThegameCharacter::ResetSkillCooldown, SkillCooldownTime, false);

	// Start the UI update timer
	GetWorld()->GetTimerManager().SetTimer(SkillCooldownUpdateTimerHandle, this, &AThegameCharacter::UpdateSkillCooldownProgress, 0.05f, true);

	// Execute skill functionality
	GetCharacterRotate();
	FVector Direction = GetActorForwardVector();
	FVector SpawnLocation = GetActorLocation() + Direction * 100;
	FRotator SpawnRotation = Direction.Rotation();

	ALightArrow* SkillArrow = Cast<ALightArrow>(GetWorld()->SpawnActor(LightArrowClass, &SpawnLocation, &SpawnRotation));
	if (SkillArrow)
	{
		// Set the skill damage on the arrow
		SkillArrow->SkillDamage = SkillDamage;
		SkillArrow->MovementComponent->Velocity = Direction * SkillArrow->MovementSpeed;
	}

	// Update UI after using the skill
	if (InGameUI)
	{
		InGameUI->UpdateValues();
	}
}
void AThegameCharacter::UpdateSkillCooldownProgress()
{
	// Increment cooldown progress based on the update interval
	SkillCooldownProgress += (0.05f / SkillCooldownTime); // 0.05 is the timer interval for the update function

	// Clamp the progress to ensure it stays between 0.0 and 1.0
	SkillCooldownProgress = FMath::Clamp(SkillCooldownProgress, 0.0f, 1.0f);

	// Update UI to reflect the current cooldown progress
	if (InGameUI)
	{
		InGameUI->UpdateValues();
	}

	// Stop updating if the cooldown is complete
	if (SkillCooldownProgress >= 1.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(SkillCooldownUpdateTimerHandle);
	}
}

void AThegameCharacter::UpdateSkill2CooldownProgress()
{
	Skill2CooldownProgress += (0.05f / Skill2CooldownTime);
	Skill2CooldownProgress = FMath::Clamp(Skill2CooldownProgress, 0.0f, 1.0f);

	if (InGameUI)
	{
		InGameUI->UpdateValues();
	}

	if (Skill2CooldownProgress >= 1.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(Skill2CooldownUpdateTimerHandle);
	}
}

void AThegameCharacter::Skill2Activate()
{
	// Mark Skill2 as being on cooldown
	bIsSkill2Ready = false;
	bIsPlayingSkill2Animation = true; // Set animation playing flag to true
	Skill2CooldownProgress = 0.0f;

	// Start the cooldown timer
	GetWorld()->GetTimerManager().SetTimer(Skill2CooldownTimerHandle, this, &AThegameCharacter::ResetSkill2Cooldown, Skill2CooldownTime, false);

	// Optional: Set a timer to update the cooldown progress in the UI
	GetWorld()->GetTimerManager().SetTimer(Skill2CooldownUpdateTimerHandle, this, &AThegameCharacter::UpdateSkill2CooldownProgress, 0.05f, true);

	// Spawn the ShieldForce actor to apply skill effect
	FVector SpawnLocation = GetActorLocation();
	FRotator SpawnRotation = GetActorRotation();
	AShieldForce* ShieldForce = GetWorld()->SpawnActor<AShieldForce>(ShieldForceClass, SpawnLocation, SpawnRotation);
	if (ShieldForce)
	{
		UE_LOG(LogTemp, Log, TEXT("ShieldForce spawned successfully for Skill2."));
	}

	// Play ShieldSkill animation if it is set
	if (ShieldSkillAnimation)
	{
		PlayAnimMontage(ShieldSkillAnimation);
		UE_LOG(LogTemp, Log, TEXT("ShieldSkill animation started."));

		// Set a timer to reset the animation flag based on the animation length
		float AnimationDuration = ShieldSkillAnimation->GetPlayLength();
		GetWorld()->GetTimerManager().SetTimer(Skill2AnimationTimerHandle, this, &AThegameCharacter::ResetSkill2AnimationFlag, AnimationDuration, false);
	}
}

void AThegameCharacter::SkillCombineActivate()
{
	if(bIsSkillComReady)
	{
		GetWorld()->GetTimerManager().SetTimer(CombineCooldownHandle, this, &AThegameCharacter::ResetCombineCooldown, CooldownTime, false);
		GetCharacterRotate();
		FVector Direction = GetActorForwardVector();
		FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 100;
		FRotator SpawnRotation = Direction.Rotation();
		ALightShieldForce* LightShieldForce = Cast<ALightShieldForce> (GetWorld()->SpawnActor(LightShieldForceClass,&SpawnLocation, &SpawnRotation));
		LightShieldForce->MovementComponent->Velocity = Direction * LightShieldForce->MovementSpeed;
		Direction.Normalize();
	}
	
	
}

//FOR SKILL COOLDOWN
void AThegameCharacter::ResetSkillCooldown()
{
	bIsSkillReady = true;
	// Ensure the progress is set to 1.0 and the UI reflects the change
	SkillCooldownProgress = 1.0f;
	// Stop the cooldown update timer if it’s running
	GetWorld()->GetTimerManager().ClearTimer(Skill2CooldownUpdateTimerHandle);
	if (InGameUI)
	{
		InGameUI->UpdateValues();
	}
}
void AThegameCharacter::ResetSkill2AnimationFlag()
{
	bIsPlayingSkill2Animation = false;
	UE_LOG(LogTemp, Log, TEXT("Skill2 animation flag reset."));
}
void AThegameCharacter::ResetSkill2Cooldown()
{
	bIsSkill2Ready = true;
	Skill2CooldownProgress = 1.0f;
	if (InGameUI)
	{
		InGameUI->UpdateValues();
	}
}

void AThegameCharacter::ResetCombineCooldown()
{
	bIsSkillComReady = true;
}

void AThegameCharacter::SkillShieldDelay()
{
	// Check if the skill is ready and if the animation is not currently playing
	if (!bIsSkill2Ready || bIsPlayingSkill2Animation)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillShield is on cooldown or already playing."));
		return; // Exit if the skill is not ready
	}

	// Call the actual skill activation function
	Skill2Activate();
}

void AThegameCharacter::HandlePause()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		if (!bIsPaused)
		{
			// Show pause menu
			if (PauseMenuWidgetClass)
			{
				UUserWidget* PauseMenuWidget = CreateWidget<UUserWidget>(PlayerController, PauseMenuWidgetClass);
				if (PauseMenuWidget)
				{
					PauseMenuWidget->AddToViewport();
					PlayerController->SetPause(true);
					PlayerController->SetInputMode(FInputModeGameAndUI());
					PlayerController->bShowMouseCursor = true;
				}
			}
		}
		else
		{
			// Hide pause menu
			PlayerController->SetPause(false);
			PlayerController->SetInputMode(FInputModeGameAndUI());
			PlayerController->bShowMouseCursor = false;
		}
		bIsPaused = !bIsPaused;
	}
}

void AThegameCharacter::ResumeGame()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		// Unpause the game and restore input for gameplay
		PlayerController->SetPause(false);
		PlayerController->SetInputMode(FInputModeGameAndUI());  // Return to game-only input
		PlayerController->bShowMouseCursor = true;  // Hide mouse cursor for gameplay

		bIsPaused = false;  // Ensure pause state is reset
	}
}
void AThegameCharacter::Dashing()
{
	if(CanDashing)
	{
		CanDashing = false;
		const FVector ForwardDirection = this->GetActorRotation().Vector();
		LaunchCharacter(ForwardDirection * DashDistance, true, true);
		PlayAnimMontage(DashAnimation);
		GetWorld()->GetTimerManager().SetTimer(DashReset, this, &AThegameCharacter::DashingReset, 1.0f, false);
	}
}

void AThegameCharacter::BeginPlay()
{
	// Set the initial progress to 1.0 and skill to be ready
	SkillCooldownProgress = 1.0f;
	bIsSkillReady = true;
	
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (MovementComponent)
	{
		float InitialSpeed = MovementComponent->MaxWalkSpeed;
		UE_LOG(LogTemp, Warning, TEXT("Initial Speed after BeginPlay: %f"), InitialSpeed);
	}
	// Call the base class  
	Super::BeginPlay();
	InGameUI = Cast<UInGameUI>(CreateWidget(GetGameInstance(), InGameUIClass));
	InGameUI->Player = this;
	InGameUI->UpdateValues();
	InGameUI->AddToViewport();
	
	
}

void AThegameCharacter::DashingReset()
{
	CanDashing = true;
}

void AThegameCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AThegameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AThegameCharacter::Move);

		//Use Weapon Skill
		EnhancedInputComponent->BindAction(WeaponSkillAction, ETriggerEvent::Started, this, &AThegameCharacter::SkillActivate);

		//Use 2 Weapon Skill		
		EnhancedInputComponent->BindAction(Weapon2SkillAction, ETriggerEvent::Started, this, &AThegameCharacter::SkillShieldDelay);

		//Use Combine Weapon Skill
		EnhancedInputComponent->BindAction(WeaponCombineSkillAction, ETriggerEvent::Started, this, &AThegameCharacter::SkillCombineActivate);

		//Dashing
		//EnhancedInputComponent->BindAction(DashingAction, ETriggerEvent::Started, this, &AThegameCharacter::Dashing);

		//Setup
		EnhancedInputComponent->BindAction(SetupAction, ETriggerEvent::Started, this, &AThegameCharacter::Setup);

		//Rewind
		EnhancedInputComponent->BindAction(RewindAction, ETriggerEvent::Started, this, &AThegameCharacter::Rewind);

	

		//Pause
		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Started, this, &AThegameCharacter::HandlePause);
		
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void AThegameCharacter::GetCharacterRotate()
{
	APlayerController* PController = GetWorld()->GetFirstPlayerController();
	

	float xMouse, yMouse;

	PController->GetMousePosition(xMouse, yMouse);

	FVector CharLoc = GetActorLocation();

	FVector2D CharInScreen;
	PController->ProjectWorldLocationToScreen(CharLoc, CharInScreen);

	FVector2D Result;
	Result.X = -(yMouse - CharInScreen.Y);
	Result.Y = xMouse - CharInScreen.X;

	// Get angle rotation and rotation Character
	float angle = FMath::RadiansToDegrees(FMath::Acos(Result.X / Result.Size()));

	if (Result.Y < 0)
		angle = 360 - angle;

	FRotator rot(0, angle, 0);

	SetActorRotation(rot);
}

void AThegameCharacter::GetDamage(float DamageAmount)
{
	CurrentHealth -= DamageAmount;
	InGameUI->UpdateValues();
	if (CurrentHealth <= 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player has died."));
		CurrentHealth = 0.0f;

		// Show death screen
		if (DeathScreenWidgetClass)
		{
			DeathScreenWidget = CreateWidget<UUserWidget>(GetWorld(), DeathScreenWidgetClass);
			if (DeathScreenWidget)
			{
				DeathScreenWidget->AddToViewport();

				// Pause the game
				APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
				if (PlayerController)
				{
					PlayerController->SetPause(true);
					// Set input mode to UI only
					FInputModeUIOnly InputMode;
					InputMode.SetWidgetToFocus(DeathScreenWidget->TakeWidget());
					PlayerController->SetInputMode(InputMode);
					PlayerController->bShowMouseCursor = true;
				}
			}
		}
	}
}

void AThegameCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		
			// find out which way is forward
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get forward vector
			const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
			// get right vector 
			const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

			// add movement 
			AddMovementInput(ForwardDirection, MovementVector.Y);
			AddMovementInput(RightDirection, MovementVector.X);
		
	}
	
}



