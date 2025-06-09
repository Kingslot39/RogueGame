// Fill out your copyright notice in the Description page of Project Settings.


#include "ShieldForce.h"
#include "Enemy.h"

#include "GameFramework/CharacterMovementComponent.h"



// Sets default values
AShieldForce::AShieldForce()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	ForceMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Force Mesh"));
	ForceMesh->SetupAttachment(RootComponent);
	
	TriggerArea = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Area"));
	TriggerArea->SetupAttachment(RootComponent);
	TriggerArea->SetBoxExtent(FVector(50,50,10));
	TriggerArea->OnComponentBeginOverlap.AddDynamic(this, &AShieldForce::OnOverlapBegin);
	SlowTime = 5.0f;
	SlowValue = 120.0f;
	

}

void AShieldForce::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Start the shield duration timer if not already started
	if (!GetWorld()->GetTimerManager().IsTimerActive(SlowCooldown))
	{
		// Activate the shield for SlowTime seconds
		GetWorld()->GetTimerManager().SetTimer(SlowCooldown, this, &AShieldForce::DestroyArea, SlowTime, false);
	}

	AEnemy* Enemy = Cast<AEnemy>(OtherActor);
	if (Enemy)
	{
		// Check if the enemy is already in the list to avoid duplicate entries
		if (!AffectedEnemies.Contains(Enemy))
		{
			// Add the enemy to the affected list
			AffectedEnemies.Add(Enemy);

			// Store the original speed and slow down the enemy
			float OriginalSpeed = Enemy->GetCharacterMovement()->MaxWalkSpeed;
			Enemy->GetCharacterMovement()->MaxWalkSpeed = SlowValue;

			// Store the original speed in a map (to restore it later)
			EnemyOriginalSpeeds.Add(Enemy, OriginalSpeed);
		}
	}
}

void AShieldForce::DestroyArea()
{
	// Restore the speed of all affected enemies
	for (AActor* Actor : AffectedEnemies)
	{
		AEnemy* EnemyActor = Cast<AEnemy>(Actor);
		if (EnemyActor && EnemyOriginalSpeeds.Contains(EnemyActor))
		{
			// Restore the original speed of each enemy
			EnemyActor->GetCharacterMovement()->MaxWalkSpeed = EnemyOriginalSpeeds[EnemyActor];
		}
	}

	// Clear the list of affected enemies and their original speeds
	AffectedEnemies.Empty();
	EnemyOriginalSpeeds.Empty();

	// Destroy the shield force area
	Destroy();
}


// Called when the game starts or when spawned
void AShieldForce::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AShieldForce::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

