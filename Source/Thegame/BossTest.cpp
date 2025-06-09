// Fill out your copyright notice in the Description page of Project Settings.


#include "BossTest.h"

#include "BossDialogue.h"
#include "NiagaraComponent.h"
#include "TimerManager.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"



// Sets default values
ABossTest::ABossTest()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MaxHealth = 1000;
	CurrentHealth = MaxHealth;
    bIsDelayDone = true;
	// Create the dialogue trigger zone
	DialogueTriggerZone = CreateDefaultSubobject<USphereComponent>(TEXT("DialogueTriggerZone"));
	if (DialogueTriggerZone)
	{
		DialogueTriggerZone->SetupAttachment(RootComponent);
		DialogueTriggerZone->SetSphereRadius(1000.0f);
		DialogueTriggerZone->SetCollisionProfileName(TEXT("Trigger"));
	}
    
}

void ABossTest::JumpAttack()
{
	NotWalking();
	if(JumpAni)
	{
		PlayAnimMontage(JumpAni,1.5f, NAME_None);
	}
	bHasJump = true;
	if(AbilityTelegraph && bHasJump){
		TargetLocation = Target->GetActorLocation() - FVector(0,0, 70);
		TargetRotator = Target->GetActorRotation();
		UNiagaraComponent* SpawnEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),AbilityTelegraph,TargetLocation, TargetRotator);
		LandingLocation = TargetLocation;
		SpawnEffect->SetFloatParameter(FName("Lifetime"), 2.0f);
		SpawnEffect->SetFloatParameter(FName("Scale"),1500);
		GetWorldTimerManager().SetTimer(JumpingDelay, this,&ABossTest::LandingAttack ,2.0f, false);
	}
}



void ABossTest::LandingAttack()
{
	//FRotator SpawnRotation = Target->GetActorRotation();
	//SetActorLocationAndRotation(LandingLocation, SpawnRotation);

	ADamageArea* DamageArea = GetWorld()->SpawnActor<ADamageArea>(DamageAreaClass, LandingLocation, TargetRotator);
	DamageArea->WaitingTime();
}

void ABossTest::DealDamage(float Damage)
{
	if (TherionGameUI)
	{
		TherionGameUI->UpdateValue();
	}
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, MaxHealth);
}

void ABossTest::CallSwordSkill()
{
	if(bIsDelayDone)
	{
	    NotWalking();
	    if(SummonAni)
	    {
		   PlayAnimMontage(SummonAni,1.0f, NAME_None );
	    }
	
		bIsDelayDone = false;
		bHasSword = true;
		const float Radius = 500.0f;
		const FVector CharacterLocation = GetActorLocation();
    
		for (int32 i = 0; i < 5; i++)
		{
			float AngleTie = i * (360.0f/5);
			float Radians = FMath::DegreesToRadians(AngleTie);

			FVector SpawnOffset = FVector(FMath::Cos(Radians), FMath::Sin(Radians),0) * Radius;
			FVector SpawnLocation = CharacterLocation + SpawnOffset;
			FRotator SpawnRotator = (CharacterLocation - SpawnLocation).Rotation() + FRotator(0,90,0);
			SpawnLocation.Z = CharacterLocation.Z - 650;

			AHolySwordBossSkill* HolySwordBossSkill = GetWorld()->SpawnActor<AHolySwordBossSkill>(HolySword, SpawnLocation, SpawnRotator);
			HolySwordBossSkill->SlashDown();
			UNiagaraComponent* SpawnEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),RecAbilityTelegraph,(SpawnLocation + FVector(0,0,500)), SpawnRotator);
			SpawnEffect->SetFloatParameter(FName("Lifetime"), 1.4f);
			SpawnEffect->SetFloatParameter(FName("Width"),400);
			SpawnEffect->SetFloatParameter(FName("Length"),1000);
		
		}
	}
	GetWorldTimerManager().SetTimer(SkillTimeDelay, this, &ABossTest::DelaySkillTime, 5.0f, false);
	
	
}

void ABossTest::SecondPhaseCallSword()
{
	
	if(bIsDelayDone)
	{
		bIsDelayDone = false;
		NotWalking();
		if(SummonAni)
	    {
		PlayAnimMontage(SummonAni,1.0f, NAME_None );
	    }
		bHasSword = true;
		const float Radius = 500.0f;
		const FVector CharacterLocation = GetActorLocation();
	

		for (int32 i = 0; i < 8; i++)
		{
			Angle = i * (360.0f/8);
			float Radians = FMath::DegreesToRadians(Angle);
			FVector SpawnOffset = FVector(FMath::Cos(Radians), FMath::Sin(Radians),0) * Radius;
			FVector SpawnLocation = CharacterLocation + SpawnOffset;

			FRotator SpawnRotator = (CharacterLocation - SpawnLocation).Rotation() + FRotator(0,90,0);
			SpawnLocation.Z = CharacterLocation.Z - 650;

			AHolySwordBossSkill* HolySwordBossSkill = GetWorld()->SpawnActor<AHolySwordBossSkill>(HolySword, SpawnLocation, SpawnRotator);
			HolySwordBossSkill->SlashDown();
			UNiagaraComponent* SpawnEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),RecAbilityTelegraph,(SpawnLocation + FVector(0,0,500)), SpawnRotator);
			SpawnEffect->SetFloatParameter(FName("Lifetime"), 1.4f);
			SpawnEffect->SetFloatParameter(FName("Width"),400);
			SpawnEffect->SetFloatParameter(FName("Length"),1000);
		}
		Angle += 10;
	}
	GetWorldTimerManager().SetTimer(SkillTimeDelay, this, &ABossTest::DelaySkillTime, 5.0f, false);
	

}

void ABossTest::SecondPhaseSpawnTree()
{
	const float Radius = 800.0f;
	const FVector CharacterLocation = GetActorLocation();
	for(int32 i = 0; i< 4;i++)
	{
		float SpawnAngle = i * (360.0f/4);
		float Radians = FMath::DegreesToRadians(SpawnAngle);
		FRotator SpawnRotation = Target->GetActorRotation();
		FVector SpawnOffset = FVector(FMath::Cos(Radians), FMath::Sin(Radians),0) * Radius;
		FVector SpawnLocation = CharacterLocation + SpawnOffset;
		SpawnLocation.Z = CharacterLocation.Z - 200;
		UNiagaraComponent* SpawnEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),AbilityTelegraph,SpawnLocation, SpawnRotation);
		SpawnEffect->SetFloatParameter(FName("Lifetime"), 1.0f);
		SpawnEffect->SetFloatParameter(FName("Scale"),1000);
		ADeadTree* DeadTree = Cast<ADeadTree>(GetWorld()->SpawnActor(DeadTreeClass,&SpawnLocation, &SpawnRotation));
		DeadTree->SetUpTree();
	}
  
	GetWorldTimerManager().SetTimer(SkillTimeDelay, this, &ABossTest::DelaySkillTime, 5.0f, false);
}



void ABossTest::SecondPhaseSpawnBush()
{
	
		for(int32 a =0 ;a < 3;a++)
		{
			float Radius = 400.0f + (a * 300.0f);
		
			const FVector CharacterLocation = GetActorLocation();
			for(int32 i = 0; i< 20;i++)
			{
				float SpawnAngle = i * (360.0f/20);
				float Radians = FMath::DegreesToRadians(SpawnAngle);
				FRotator SpawnRotation = Target->GetActorRotation();
				FVector SpawnOffset = FVector(FMath::Cos(Radians), FMath::Sin(Radians),0) * Radius;
				FVector SpawnLocation = CharacterLocation + SpawnOffset;
				SpawnLocation.Z = CharacterLocation.Z - 600;
				ADeadlyBush* DeadlyBush = Cast<ADeadlyBush>(GetWorld()->SpawnActor(DeadBushClass,&SpawnLocation, &SpawnRotation));
				DeadlyBush->WaitForDestroy();
			}
		
		}
	   GetWorldTimerManager().SetTimer(SkillTimeDelay, this, &ABossTest::DelaySkillTime, 5.0f, false);
	
}

void ABossTest::SpawnBush()
{

		
		const float Radius = 600.0f;
		const FVector CharacterLocation = GetActorLocation();
		for(int32 i = 0; i< 20;i++)
		{
			float SpawnAngle = i * (360.0f/20);
			float Radians = FMath::DegreesToRadians(SpawnAngle);
			FRotator SpawnRotation = Target->GetActorRotation();
			FVector SpawnOffset = FVector(FMath::Cos(Radians), FMath::Sin(Radians),0) * Radius;
			FVector SpawnLocation = CharacterLocation + SpawnOffset;
			SpawnLocation.Z = CharacterLocation.Z - 600;
			ADeadlyBush* DeadlyBush = Cast<ADeadlyBush>(GetWorld()->SpawnActor(DeadBushClass,&SpawnLocation, &SpawnRotation));
			DeadlyBush->WaitForDestroy();
		}
	    GetWorldTimerManager().SetTimer(SkillTimeDelay, this, &ABossTest::DelaySkillTime, 5.0f, false);
}

void ABossTest::Shoot(FVector Direction)
{
	if(bIsDelayDone)
	{
		NotWalking();
		if(FireAni)
		{
			PlayAnimMontage(FireAni, 1.0f, NAME_None);
		}
		FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 100;
		FRotator SpawnRotation = Direction.Rotation() - FRotator(-10, 90,0);
		ABossSpell* SpawnedBullet = Cast<ABossSpell>(GetWorld()->SpawnActor(BulletClass, &SpawnLocation, &SpawnRotation));
		Direction.Normalize();
		SpawnedBullet->MovementComponent->Velocity = Direction * SpawnedBullet->MovementSpeed;
		UNiagaraComponent* SpawnEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),RecAbilityTelegraph,SpawnLocation, SpawnRotation);
		SpawnEffect->SetFloatParameter(FName("Lifetime"), 0.5f);
		SpawnEffect->SetFloatParameter(FName("Width"),300);
		SpawnEffect->SetFloatParameter(FName("Length"),1000);
	}
	GetWorldTimerManager().SetTimer(SkillTimeDelay, this, &ABossTest::DelaySkillTime, 5.0f, false);
}

void ABossTest::TelegraphForBush()
{
	if(bIsDelayDone)
	{
		bIsDelayDone = false;
		const float Radius = 600.0f;
		const FVector CharacterLocation = GetActorLocation();
		for(int32 i = 0; i< 20;i++)
		{
			float SpawnAngle = i * (360.0f/20);
			float Radians = FMath::DegreesToRadians(SpawnAngle);
			FRotator SpawnRotation = Target->GetActorRotation();
			FVector SpawnOffset = FVector(FMath::Cos(Radians), FMath::Sin(Radians),0) * Radius;
			FVector SpawnLocation = CharacterLocation + SpawnOffset;
			UNiagaraComponent* SpawnEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),AbilityTelegraph,SpawnLocation, SpawnRotation);
			SpawnEffect->SetFloatParameter(FName("Lifetime"), 1.0f);
			SpawnEffect->SetFloatParameter(FName("Scale"),200);
		}
		NotWalking();
		if(SummonAni)
		{
			PlayAnimMontage(SummonAni,1.0f, NAME_None);
		}
		GetWorldTimerManager().SetTimer(TelegraphDelay,this, &ABossTest::SpawnBush, 1.3f, false);
	}
}

void ABossTest::TelegraphBushDelay2Phase()
{
	if(bIsDelayDone)
	{
		bIsDelayDone = false;
		for(int32 a =0 ;a < 3;a++)
		{
			float Radius = 400.0f + (a * 300.0f);
		
			const FVector CharacterLocation = GetActorLocation();
			for(int32 i = 0; i< 20;i++)
			{
				float SpawnAngle = i * (360.0f/20);
				float Radians = FMath::DegreesToRadians(SpawnAngle);
				FRotator SpawnRotation = Target->GetActorRotation();
				FVector SpawnOffset = FVector(FMath::Cos(Radians), FMath::Sin(Radians),0) * Radius;
				FVector SpawnLocation = CharacterLocation + SpawnOffset;
				UNiagaraComponent* SpawnEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),AbilityTelegraph,SpawnLocation, SpawnRotation);
				SpawnEffect->SetFloatParameter(FName("Lifetime"), 1.0f);
				SpawnEffect->SetFloatParameter(FName("Scale"),200);
			}
			NotWalking();
			if(SummonAni)
			{
				PlayAnimMontage(SummonAni,1.0f, NAME_None);
			}
			GetWorldTimerManager().SetTimer(TelegraphDelayBushPhase, this, &ABossTest::SecondPhaseSpawnBush, 1.3f, false);
		}
	}
}

void ABossTest::TelegraphTree()
{
	if(bIsDelayDone)
	{
		bIsDelayDone = false;
		SpawnTreeLocation = Target->GetActorLocation();
		SpawnTreeRotator = Target->GetActorRotation();
		UNiagaraComponent* SpawnEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),AbilityTelegraph,SpawnTreeLocation, SpawnTreeRotator);
		SpawnEffect->SetFloatParameter(FName("Lifetime"), 1.3f);
		SpawnEffect->SetFloatParameter(FName("Scale"),1000);
		NotWalking();
		if(SummonAni)
		{
			PlayAnimMontage(SummonAni,1.0f, NAME_None);
		}
		GetWorldTimerManager().SetTimer(TelegraphTreeDelay,this, &ABossTest::SpawnTree, 1.3,false);
	}
}

void ABossTest::TelegraphTreePhase2()
{
	if(bIsDelayDone)
	{
		bIsDelayDone = false;
		const float Radius = 800.0f;
		const FVector CharacterLocation = GetActorLocation();
		for(int32 i = 0; i< 4;i++)
		{
			float SpawnAngle = i * (360.0f/4);
			float Radians = FMath::DegreesToRadians(SpawnAngle);
			FRotator SpawnRotation = Target->GetActorRotation();
			FVector SpawnOffset = FVector(FMath::Cos(Radians), FMath::Sin(Radians),0) * Radius;
			FVector SpawnLocation = CharacterLocation + SpawnOffset;
			SpawnLocation.Z = CharacterLocation.Z - 200;
			UNiagaraComponent* SpawnEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),AbilityTelegraph,SpawnLocation, SpawnRotation);
			SpawnEffect->SetFloatParameter(FName("Lifetime"), 1.3f);
			SpawnEffect->SetFloatParameter(FName("Scale"),1000);
		}
		NotWalking();
		if(SummonAni)
		{
			PlayAnimMontage(SummonAni,1.0f, NAME_None);
		}
		GetWorldTimerManager().SetTimer(TelegraphTree2Phase,this, &ABossTest::SecondPhaseSpawnTree, 1.3,false);
	}
}

void ABossTest::SpawnTree()
{
	
	ADeadTree* DeadTree = Cast<ADeadTree>(GetWorld()->SpawnActor(DeadTreeClass,&SpawnTreeLocation, &SpawnTreeRotator));
	DeadTree->SetUpTree();
	
}



void ABossTest::NotWalking()
{
	GetCharacterMovement()->DisableMovement();
	GetWorldTimerManager().SetTimer(NoneMoveCoolDown, this, &ABossTest::ContinueWalking, 3.0f, false);
}

void ABossTest::ContinueWalking()
{
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

void ABossTest::DelaySkillTime()
{
	bIsDelayDone = true;
}

void ABossTest::DotDamage(float Damage)
{
	UGameplayStatics::ApplyDamage(this, Damage, nullptr, this, nullptr);
}

void ABossTest::OnTriggerZoneEntered(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((!bDialogueTriggered || bAllowMultipleInteractions) && !bDialogueFinished)
    {
        AThegameCharacter* PlayerCharacter = Cast<AThegameCharacter>(OtherActor);
        if (PlayerCharacter && PlayerCharacter == Target)
        {
            // Player has entered the trigger zone, start dialogue
            bDialogueTriggered = true;
            
            // Create and initialize the dialogue widget
            APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
            if (PlayerController && DialogueWidgetClass && DialogueLines.Num() > 0)
            {
                UBossDialogue* BossDialogueWidget = CreateWidget<UBossDialogue>(PlayerController, DialogueWidgetClass);
                if (BossDialogueWidget)
                {
                    BossDialogueWidget->SetOwningBoss(this);
                    BossDialogueWidget->InitializeDialogue(this, DialogueLines, DialogueDisplayTime);
                    BossDialogueWidget->StartDialogue();
                }
            }
            else
            {
                // No dialogue to show or missing components, skip to battle
                bDialogueFinished = true;
            }
        }
    }
}

// Called when the game starts or when spawned
void ABossTest::BeginPlay()
{
	Super::BeginPlay();
	// Find the player character - add null check
	if (GetWorld() && GetWorld()->GetFirstPlayerController())
	{
		Target = Cast<AThegameCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	}
    
	// Create the health UI - add null checks
	if (TherionHealthBar)
	{
		TherionGameUI = CreateWidget<UTherionGameUI>(GetGameInstance(), TherionHealthBar);
		if (TherionGameUI)
		{
			TherionGameUI->Target = this;
			TherionGameUI->UpdateValue();
			TherionGameUI->AddToViewport();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to create TherionGameUI"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("TherionHealthBar class not set"));
	}
    
	// Just bind the overlap event if it exists
	if (DialogueTriggerZone)
	{
		DialogueTriggerZone->OnComponentBeginOverlap.AddDynamic(this, &ABossTest::OnTriggerZoneEntered);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("DialogueTriggerZone is null in BeginPlay"));
	}
}

void ABossTest::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
    
	// Check if the boss should be destroyed after second phase dialogue
	if (bSecondPhaseDialogueFinished)
	{
		// The boss can now be destroyed
		Destroy();
		return;
	}
    
	// Only perform boss battle logic if first dialogue is finished AND second dialogue hasn't started
	if (bDialogueFinished && !bSecondPhaseDialogueTriggered)
	{
		// Add TherionGameUI here if needed
		if (TherionGameUI && !TherionGameUI->IsInViewport())
		{
			TherionGameUI->AddToViewport();
		}
        
		// Existing boss battle logic
		if (CurrentHealth <= 500 && !bHasJump)
		{
			JumpAttack();
		}
        
		// Check if the boss is defeated
		if (CurrentHealth <= 0)
		{
			// Call the death handler instead of immediately destroying
			HandleDeath();
		}
	}
	else if (!bDialogueFinished || bSecondPhaseDialogueTriggered)
	{
		// We're in some form of dialogue - either first or second phase
		// Hide player UI during any dialogue
		if (Target && Target->InGameUI && Target->InGameUI->IsInViewport())
		{
			Target->InGameUI->RemoveFromParent();
		}
        
		// IMPORTANT: Ensure boss doesn't move or attack during dialogues
		GetCharacterMovement()->DisableMovement();
        
		// Cancel any ongoing skills or timers if in second phase dialogue
		if (bSecondPhaseDialogueTriggered && !bSecondPhaseDialogueFinished)
		{
			// This additional check ensures the boss stays inactive during second phase dialogue
			StopAnimMontage();
		}
	}
}

// Called every frame


// Called to bind functionality to input
void ABossTest::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABossTest::TriggerSecondPhaseDialogue()
{
	if (!bSecondPhaseDialogueTriggered && SecondPhaseDialogueLines.Num() > 0)
	{
		bSecondPhaseDialogueTriggered = true;
        
		// Create and initialize the dialogue widget
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		if (PlayerController && DialogueWidgetClass)
		{
			UBossDialogue* BossDialogueWidget = CreateWidget<UBossDialogue>(PlayerController, DialogueWidgetClass);
			if (BossDialogueWidget)
			{
				// Disable movement for boss and player during dialogue
				GetCharacterMovement()->DisableMovement();
                
				if (Target && Target->GetCharacterMovement())
				{
					Target->GetCharacterMovement()->DisableMovement();
				}
                
				// Hide player UI during dialogue
				if (Target && Target->InGameUI && Target->InGameUI->IsInViewport())
				{
					Target->InGameUI->RemoveFromParent();
				}
                
				// Initialize and start the second phase dialogue
				BossDialogueWidget->SetOwningBoss(this);
				BossDialogueWidget->InitializeDialogue(this, SecondPhaseDialogueLines, DialogueDisplayTime);
				BossDialogueWidget->StartDialogue();
			}
		}
	}
}

void ABossTest::HandleDeath()
{
	// Prevent any further attacks or skills
	bSecondPhaseDialogueTriggered = true;
    
	// Cancel any ongoing skills or timers
	GetWorldTimerManager().ClearAllTimersForObject(this);
    
	// Stop any ongoing animations
	StopAnimMontage();
    
	// Disable boss movement
	GetCharacterMovement()->DisableMovement();
    
	// Trigger the second phase dialogue
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController && DialogueWidgetClass && SecondPhaseDialogueLines.Num() > 0)
	{
		UBossDialogue* BossDialogueWidget = CreateWidget<UBossDialogue>(PlayerController, DialogueWidgetClass);
		if (BossDialogueWidget)
		{
			// Disable player movement during dialogue
			if (Target && Target->GetCharacterMovement())
			{
				Target->GetCharacterMovement()->DisableMovement();
			}
            
			// Hide player UI during dialogue
			if (Target && Target->InGameUI && Target->InGameUI->IsInViewport())
			{
				Target->InGameUI->RemoveFromParent();
			}
            
			// Initialize and start the second phase dialogue
			BossDialogueWidget->SetOwningBoss(this);
			BossDialogueWidget->InitializeDialogue(this, SecondPhaseDialogueLines, DialogueDisplayTime);
			BossDialogueWidget->StartDialogue();
		}
	}
	
}