// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawner.h"

#include "MyGameInstance.h"
#include "TeleportGate.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "TeleportGate.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "NavigationSystem.h"


// Sets default values
AEnemySpawner::AEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Create the spawn area component
	SpawnArea = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnArea"));
	SpawnArea->SetupAttachment(RootComponent);

	// Set default size for the spawn area
	SpawnArea->SetBoxExtent(FVector(500.f, 500.f, 500.f)); // Adjust as needed
	SpawnArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemySpawner::SpawnEnemy()
{
	// Do not spawn if we've reached the maximum allowed spawns.
	if (SpawnCount >= MaxEnemySpawn)
	{
		return;
	}

	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), EnemyClass, Enemies);

	// Check concurrent limit (e.g., maximum 5 enemies at a time)
	if (EnemyClass && Enemies.Num() < MaxEnemies)
	{
		const int32 MaxAttempts = 10;
		bool bSpawned = false;

		for (int32 Attempt = 0; Attempt < MaxAttempts; Attempt++)
		{
			FVector Origin = SpawnArea->Bounds.Origin;
			FVector Extent = SpawnArea->Bounds.BoxExtent;
			FVector SpawnLocation = UKismetMathLibrary::RandomPointInBoundingBox(Origin, Extent);
			FRotator SpawnRotation = GetActorRotation();

			FCollisionQueryParams QueryParams;
			QueryParams.bTraceComplex = true;
			QueryParams.bReturnPhysicalMaterial = false;
			QueryParams.AddIgnoredActor(this);

			FCollisionShape CollisionShape = FCollisionShape::MakeSphere(EnemyCollisionRadius);

			bool bOverlap = GetWorld()->OverlapAnyTestByChannel(
				SpawnLocation,
				FQuat::Identity,
				ECollisionChannel::ECC_Pawn,
				CollisionShape,
				QueryParams
			);

			if (!bOverlap)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				GetWorld()->SpawnActor(EnemyClass, &SpawnLocation, &SpawnRotation, SpawnParams);
				SpawnCount++;  // Increase total spawned count.
				bSpawned = true;
				break;
			}
		}

		if (!bSpawned)
		{
			UE_LOG(LogTemp, Warning, TEXT("Could not find a suitable spawn location after %d attempts."), MaxAttempts);
		}
	}
}
void AEnemySpawner::OpenLevelTrigger()
{
	TArray<AActor*> GateActor;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATeleportGate::StaticClass(), GateActor );
	for(AActor* Actor : GateActor)
	{
		TeleportGate = Cast<ATeleportGate>(Actor);
		if(TeleportGate)
		{
			FVector Location = TeleportGate->GetActorLocation();
			GetWorld()->SpawnActor(LevelTriggerClass, &Location);
		}
	}

	
}



// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
    
	GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &AEnemySpawner::SpawnEnemy, SpawnFrequency, true);
}

// Called every frame
void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

