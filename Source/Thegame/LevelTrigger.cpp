#include "LevelTrigger.h"
#include "MyGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/PlayerController.h"
#include "NiagaraFunctionLibrary.h"
#include "WeaponSystem.h"
#include "ThegameCharacter.h"


ALevelTrigger::ALevelTrigger()
{
    // Primary Actor Ticking should be enabled for consistent movement over time. To have movement.
    PrimaryActorTick.bCanEverTick = true;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
    LevelTriggerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Level Trigger Area"));
    LevelTriggerMesh->SetupAttachment(RootComponent);
    TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Volume"));
    RootComponent = TriggerVolume;
    TriggerVolume->SetCollisionProfileName(TEXT("OverlapAll"));

    MapName.Add(FName("woodlands_RuinsLight"));
    MapName.Add(FName("Woodlands_Mountain"));
    MapName.Add(FName("Woodlands_MountainDry"));
    MapName.Add(FName("woodlands_RuinsDark"));
}
 void ALevelTrigger::StartLevelStreaming(UPrimitiveComponent* HitComp, AActor* OtherActor,
     UPrimitiveComponent* OtherComp, int32 BodyIndex, bool bSweep, const FHitResult& Hit)
 {
 	// Cast OtherActor to AThegameCharacter, to replace for your Player Controlled pawn type.
 	if (AThegameCharacter* Character = Cast<AThegameCharacter>(OtherActor))
 	{

        //Get Player Controller
 		MyPlayerController= UGameplayStatics::GetPlayerController(GetWorld(), 0);
 		if (MyPlayerController)
 		{
 			//Disable input movement of the theCharacter by setting.
            MyCharacter = Cast<AThegameCharacter>(MyPlayerController->GetPawn());
			MyPlayerController->SetIgnoreMoveInput(true);
 			MyPlayerController->SetIgnoreLookInput(true);

 			// Store the current camera location
 			APlayerCameraManager* CameraManager = MyPlayerController->PlayerCameraManager;
 			if (CameraManager)
 			{
 				OriginalCameraLocation = CameraManager->GetCameraLocation();
 				OriginalCameraRotation = CameraManager->GetCameraRotation();

                CameraManager->SetActorLocationAndRotation( OriginalCameraLocation, OriginalCameraRotation);
 			}

            //Set Timers to account for world changing
            GetWorldTimerManager().SetTimer(MovementTimerHandle, this, &ALevelTrigger::ContinueForwardMovement, 1.0f, false);
 		}
    }
}
  void ALevelTrigger::ContinueForwardMovement()
  {
  if (MyPlayerController)
  {
    if (AThegameCharacter* Character = Cast<AThegameCharacter>(MyPlayerController->GetPawn()))
	{
        //Continue movement code
	   FVector ForwardVector = Character->GetActorForwardVector();
       Character -> AddMovementInput(ForwardVector,1.0f, true);
       
		// After 1 seconds of movement and fade-out, load the next level
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &ALevelTrigger::FadeOutAndLoad, 1.0f, false);
    
	}
  }
}

void ALevelTrigger::FadeOutAndLoad()
{
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PlayerController)
    {
        //Disables input of character movement.
        PlayerController->SetIgnoreMoveInput(true);
         PlayerController->SetIgnoreLookInput(true);
         APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
    if (CameraManager)    
        {
            //Start the fade-out
            CameraManager->StartCameraFade(0.f, 1.f, 1.0f, FLinearColor::Black, false, true);

            //Delay the next world loading.
            FTimerHandle TimerHandleLevel;
            GetWorld()->GetTimerManager().SetTimer(TimerHandleLevel, this, &ALevelTrigger::LoadNextLevel, 1.0f, false);
        }
    }
}

// Step 1: Add debug logs to LevelTrigger.cpp in the LoadNextLevel function
void ALevelTrigger::LoadNextLevel()
{
    // Select a random level to load
    int32 RandomIndex = FMath::RandRange(0, MapName.Num() - 1);
    FName SelectedMap = MapName[RandomIndex];
    
    // Get game instance
    UMyGameInstance* GI = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (GI)
    {
        // Store the player's state to be applied in the new level
        APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        if (PlayerController)
        {
            AThegameCharacter* PlayerCharacter = Cast<AThegameCharacter>(PlayerController->GetPawn());
            if (PlayerCharacter)
            {
                // DEBUG: Log initial weapon state
                if (PlayerCharacter->WeaponSystem) {
                    UE_LOG(LogTemp, Warning, TEXT("BEFORE SAVE: Character weapons - Slot1=%d, Slot2=%d"), 
                        (int32)PlayerCharacter->WeaponSystem->ActiveWeaponSlot1, 
                        (int32)PlayerCharacter->WeaponSystem->ActiveWeaponSlot2);
                }
                
                // Save basic player stats
                GI->SavedPlayerHealth = PlayerCharacter->CurrentHealth;
                GI->SavedMaxHealth = PlayerCharacter->MaxHealth;
                GI->SavedSpeedMultiplier = PlayerCharacter->SpeedMultiplier;
                GI->SavedBaseSpeed = PlayerCharacter->BaseSpeed;
                
                // SAVE BUFF STATES
                GI->SavedHasRotatingShield = PlayerCharacter->bHasRotatingShield;
                GI->SavedHasReroll = PlayerCharacter->bHasReroll;
                GI->SavedSkillDamage = PlayerCharacter->SkillDamage;
                GI->SavedBaseSkillCooldownTime = PlayerCharacter->BaseSkillCooldownTime;
                
                // DEBUG: Log the rotating shield state
                UE_LOG(LogTemp, Warning, TEXT("LevelTrigger: Saving buff states - HasRotatingShield=%d, HasReroll=%d"), 
                    PlayerCharacter->bHasRotatingShield, PlayerCharacter->bHasReroll);
                
                // Save weapon selection information - critical fix area
                if (PlayerCharacter->WeaponSystem)
                {
                    // Critical: Use explicit HARD-CODED integer values based on weapon types
                    // This bypasses any potential issues with enum conversion
                    int32 WeaponSlot1AsInt = static_cast<int32>(PlayerCharacter->WeaponSystem->ActiveWeaponSlot1);
                    int32 WeaponSlot2AsInt = static_cast<int32>(PlayerCharacter->WeaponSystem->ActiveWeaponSlot2);
    
                    // DEBUG: Log what the integer values actually are
                    UE_LOG(LogTemp, Error, TEXT("PRE-SAVE RAW INT VALUES: Slot1=%d, Slot2=%d"), 
                        WeaponSlot1AsInt, WeaponSlot2AsInt);
    
                    // Save the raw integer values
                    GI->SavedWeaponSlot1AsInt = WeaponSlot1AsInt;
                    GI->SavedWeaponSlot2AsInt = WeaponSlot2AsInt;
    
                    // Save the weapon types as usual for compatibility
                    GI->SavedActiveWeaponSlot1 = PlayerCharacter->WeaponSystem->ActiveWeaponSlot1;
                    GI->SavedActiveWeaponSlot2 = PlayerCharacter->WeaponSystem->ActiveWeaponSlot2;
    
                    // Debug: Log the values we're saving
                    UE_LOG(LogTemp, Error, TEXT("SAVING RAW INTEGER WEAPON VALUES: Slot1=%d, Slot2=%d"), 
                        GI->SavedWeaponSlot1AsInt, GI->SavedWeaponSlot2AsInt);
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("WeaponSystem not found when saving for level transition"));
                    GI->SavedActiveWeaponSlot1 = EWeaponType::None;
                    GI->SavedActiveWeaponSlot2 = EWeaponType::None;
                }
                
                GI->bShouldApplySavedPlayerStats = true;
                
                // DEBUG: Final check of saved values
                UE_LOG(LogTemp, Warning, TEXT("GAME INSTANCE AFTER SAVE: Weapon values - Slot1=%d, Slot2=%d, HasRotatingShield=%d"), 
                    (int32)GI->SavedActiveWeaponSlot1, (int32)GI->SavedActiveWeaponSlot2, GI->SavedHasRotatingShield);
            }
        }
    }
    
    UE_LOG(LogTemp, Log, TEXT("Transitioning to new level: %s"), *SelectedMap.ToString());
    
    // Load the selected level
    UGameplayStatics::OpenLevel(this, SelectedMap);
}

// Called when the game starts or when spawned
void ALevelTrigger::BeginPlay()
{
	Super::BeginPlay();
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ALevelTrigger::StartLevelStreaming);
     TriggerVolume->SetGenerateOverlapEvents(true);

    if(LevelTriggerClass) {
	    UNiagaraComponent* SpawnedEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), LevelTriggerClass, GetActorLocation(), GetActorRotation());
    }
}

// Called every frame
void ALevelTrigger::Tick(float DeltaTime)  //If it's on the Player.
{
	Super::Tick(DeltaTime);
	
}
