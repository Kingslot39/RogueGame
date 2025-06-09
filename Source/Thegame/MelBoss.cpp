// Fill out your copyright notice in the Description page of Project Settings.


#include "MelBoss.h"

#include <ThirdParty/ShaderConductor/ShaderConductor/External/SPIRV-Headers/include/spirv/unified1/spirv.h>

#include "NiagaraComponent.h"
#include "EntitySystem/MovieSceneEntitySystemRunner.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AMelBoss::AMelBoss()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MaxHealth = 3000;
	CurrentHealth = MaxHealth;
	bIsDelayDone = true;

}

void AMelBoss::DealDamage(float Damage)
{
	if(MelJudmentWidget)
	{
		MelJudmentWidget->UpdateValue();
	}
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, MaxHealth);
}

void AMelBoss::SpawnSword()
{
	UnAbleToMove();
	SwordSpawned = 0;
	GetWorldTimerManager().SetTimer(SwordSpawnTimerHandle, this, &AMelBoss::JudmentSwordRush, 1.2f, true);
}



void AMelBoss::JudmentSwordRush()
{
	
	if(bIsDelayDone)
	{
		bIsDelayDone = false;
		const float Radius = 300.0f;
		const FVector CharacterLocation = GetActorLocation();
    
		for (int32 i = 0; i < 5; i++)
		{
			float AngleTie = i * (360.0f/5);
			float Radians = FMath::DegreesToRadians(AngleTie);

			FVector SpawnOffset = FVector(FMath::Cos(Radians), FMath::Sin(Radians),0) * Radius;
			FVector SpawnLocation = CharacterLocation + SpawnOffset;
			FRotator SpawnRotator = (CharacterLocation - SpawnLocation).Rotation() + FRotator(90,180,90);

			AJudgmentSwordSlice* JudgmentSwordSlice = GetWorld()->SpawnActor<AJudgmentSwordSlice>(SwordClass, SpawnLocation, SpawnRotator);
		}
	}
	GetWorldTimerManager().SetTimer(SkillTimeDelay, this, &AMelBoss::DelaySkillTime, 2.0f, false);
	
}

void AMelBoss::WaitingToRush()
{
	bIsCanRush = true;
}
void AMelBoss::UmbrellaBloomSkill()
{
	
	
}

void AMelBoss::UmbrellaBloomTelegraph()
{
	if(bIsDelayDone)
	{
		bIsDelayDone = false;
		FVector PlayerLocation = GetActorLocation();
		float Radius = 600; // radius around the player to spawn

		for (int32 i = 0; i < 6; ++i)
		{
			// Generate a random point in a circle around the player
			float Angle = FMath::RandRange(0.f, 2 * PI);
			float Distance = FMath::RandRange(500.0f, Radius);
			FVector Offset = FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0.f) * Distance;
			SpawnUmbrellaLocation = PlayerLocation + Offset;
			SpawnUmbrellaRotation = FRotator::ZeroRotator;
			UNiagaraComponent* SpawnEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),AbilityTelegraph,SpawnUmbrellaLocation, SpawnUmbrellaRotation);
			SpawnEffect->SetFloatParameter(FName("Lifetime"), 1.0f);
			SpawnEffect->SetFloatParameter(FName("Scale"),400);
			AUmbrellaBloom* UmbrellaBloom =  GetWorld()->SpawnActor<AUmbrellaBloom>(UmbrellaClass, SpawnUmbrellaLocation, SpawnUmbrellaRotation);
			UmbrellaBloom->SkillRun();
		}
		
	}
	GetWorldTimerManager().SetTimer(SkillTimeDelay, this, &AMelBoss::DelaySkillTime, 2.0f, false);
	
}

void AMelBoss::ScytheSlash()
{
	if(bIsDelayDone)
	{
		bIsDelayDone = false;
		FVector SpawnLocation = GetActorLocation()- FVector(0,0,80);
		FRotator SpawnRotation = GetActorRotation();

		AUmbrellaScythe* UmbrellaScythe = Cast<AUmbrellaScythe>(GetWorld()->SpawnActor(ScytheClass, &SpawnLocation, &SpawnRotation));
		UmbrellaScythe->ScytheActivate();
	}
	GetWorldTimerManager().SetTimer(SkillTimeDelay, this, &AMelBoss::DelaySkillTime, 2.0f, false);
}

void AMelBoss::SlashHammer()
{
	if(bIsDelayDone)
	{
		bIsDelayDone = false;
		FVector Forward = GetActorForwardVector();
		FVector Right = GetActorRightVector(); // Direction to the right of the character
		FVector Origin = GetActorLocation();
		float ForwardDistance = 200.f; // how far in front of the character
		float SideSpacing = 500.f;     // spacing between objects left-to-right

		// Offsets: -1 (left), 0 (center), +1 (right)
		TArray<int32> SideOffsets = {-1, 0, 1};

		for (int32 Offset : SideOffsets)
		{
			FVector SpawnPos = Origin + (Forward * ForwardDistance) + (Right * Offset * SideSpacing) - FVector(0,0,20);
			FRotator SpawnRot = Forward.Rotation() - FRotator(0,90,90);
			GetWorld()->SpawnActor(HammerClass, &SpawnPos, &SpawnRot);
			GetWorld()->SpawnActor(HammerClass, &SpawnPos, &SpawnRot);
		}
	}
	GetWorldTimerManager().SetTimer(SkillTimeDelay, this, &AMelBoss::DelaySkillTime, 2.0f, false);
	
}

void AMelBoss::GodSlamDown()
{
	if(bIsDelayDone)
	{
		bIsDelayDone = false;
		FVector SpawnLocation = GetActorLocation() + FVector(0,0,100);
		FRotator SpawnRotator = GetActorRotation() - FRotator(0,90,0);
		GetWorld()->SpawnActor(GodHammerClass, &SpawnLocation, &SpawnRotator);
	}
	GetWorldTimerManager().SetTimer(SkillTimeDelay, this, &AMelBoss::DelaySkillTime, 2.0f, false);
}

void AMelBoss::UnAbleToMove()
{
	GetCharacterMovement()->DisableMovement();
	GetWorldTimerManager().SetTimer(StartMoving, this, &AMelBoss::AbleToMove, 3.0f, false);
	
}

void AMelBoss::AbleToMove()
{
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

int AMelBoss::GetSpeed()
{
	int speed = GetCharacterMovement()->GetMaxSpeed();
	return  speed;
}

void AMelBoss::DelaySkillTime()
{
	bIsDelayDone = true;
}

// Called when the game starts or when spawned
void AMelBoss::BeginPlay()
{
	Super::BeginPlay();
	Target = Cast<AThegameCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (MelJudmentWidget)
	{
		MelJudmentWidget = CreateWidget<UMelJudmentWidget>(GetGameInstance(), MelHealthBar);
		if (MelJudmentWidget)
		{
			MelJudmentWidget->Target = this;
			MelJudmentWidget->UpdateValue();
			MelJudmentWidget->AddToViewport();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to create MelUI"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MelHealthBar class not set"));
	}
}

// Called every frame
void AMelBoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMelBoss::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

