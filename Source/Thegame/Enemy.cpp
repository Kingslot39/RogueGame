 // Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "BrainComponent.h"
#include "EnemyBTController.h"
#include "EnemyHealthWidget.h"
#include "Engine.h"
#include "LevelTrigger.h"
#include "ThegameGameMode.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	HealthWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthWidget"));
	HealthWidgetComponent->SetupAttachment(RootComponent);
	HealthWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	HealthWidgetComponent->SetDrawSize(FVector2D(150, 50));

	


}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	Player = Cast<AThegameCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AEnemy::OnHit);
	GetCharacterMovement()->MaxWalkSpeed = 400.f;
	if (HealthWidgetComponent)
	{
		HealthWidgetComponent->SetWidgetClass(EnemyHealthWidgetBPClass); // Ensure this is set correctly in the editor
		HealthWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 100.f));  // Position it above the enemy's head
		HealthWidgetComponent->InitWidget();

		// Immediately set initial health
		UEnemyHealthWidget* HealthWidget = Cast<UEnemyHealthWidget>(HealthWidgetComponent->GetUserWidgetObject());
		if (HealthWidget)
		{
			float InitialHealthPercentage = CurrentHealth / MaxHealth;
			HealthWidget->UpdateHealthBar(InitialHealthPercentage);  // Set initial health value
		}
	}
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//if (Player)  // Ensure Player is valid
	//{
	//	FVector TowardsPlayer = Player->GetActorLocation() - GetActorLocation();
	//	TowardsPlayer.Normalize();  // Normalize the vector to get a direction
	//	AddMovementInput(TowardsPlayer);
		
	//}
	
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


void AEnemy::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector Normal,
	const FHitResult& Hit)
{
	

}

void AEnemy::Ragdoll()
{
	Cast<AEnemyBTController>(GetController())->BrainComponent->PauseLogic("Ragdolling!");
	GetMesh()->SetCollisionProfileName("Ragdoll");
	GetMesh()->SetSimulatePhysics(true);
	GetCapsuleComponent()->SetCollisionProfileName("NoCollision");
	GetWorld()->GetTimerManager().SetTimer(RagdollTimerHandle, this, &AEnemy::StopRagdoll, RagdollTime, false);
	


}

void AEnemy::StopRagdoll()
{
	
	if(CurrentHealth <= 0)
	{
		Destroy();
		return;
		
	}
	GetMesh()->SetSimulatePhysics(false);
	GetMesh()->SetCollisionProfileName("CharacterMesh");
	GetCapsuleComponent()->SetWorldLocation(GetMesh()->GetSocketLocation("pelvis"));
	GetMesh()->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
	GetMesh()->SetRelativeLocationAndRotation(FVector(0,0,-90), FRotator(0,-90,0));
	GetCapsuleComponent()->SetCollisionProfileName("Pawn");
	GetCharacterMovement()->GravityScale = 1;
	Cast<AEnemyBTController>(GetController())->BrainComponent->ResumeLogic("Moving Again!");
	

}



void AEnemy::GetDamage(float DamageAmount)
{
	CurrentHealth -= DamageAmount;
	float HealthPercentage = CurrentHealth / MaxHealth;

	if (HealthWidgetComponent)
	{
		UEnemyHealthWidget* HealthWidget = Cast<UEnemyHealthWidget>(HealthWidgetComponent->GetUserWidgetObject());
		if (HealthWidget)
		{
			HealthWidget->UpdateHealthBar(HealthPercentage);
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::FromInt(CurrentHealth));
	UE_LOG(LogTemp, Warning, TEXT("Enemy took damage. Current Health: %f"), CurrentHealth);

	if (CurrentHealth <= 0.0f)
	{
		AThegameGameMode* GameMode = Cast<AThegameGameMode>(UGameplayStatics::GetGameMode(this));
		if (GameMode)
		{
			UE_LOG(LogTemp, Warning, TEXT("Calling HandleEnemyDeath() in GameMode."));
			GameMode->HandleEnemyDeath();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("GameMode is not of type AThegameGameMode!"));
		}

		Destroy();
	}
}

