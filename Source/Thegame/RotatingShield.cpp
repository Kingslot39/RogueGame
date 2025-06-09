// Fill out your copyright notice in the Description page of Project Settings.


#include "RotatingShield.h"
#include "ThegameCharacter.h"
#include "Enemy.h"
#include "BossTest.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "Engine/Engine.h"

// Sets default values
ARotatingShield::ARotatingShield()
{
	// Set this actor to call Tick() every frame
	PrimaryActorTick.bCanEverTick = true;
    
	// Create root component
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	// Create shield mesh
	ShieldMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShieldMesh"));
	ShieldMesh->SetupAttachment(SceneRoot);
	ShieldMesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
    
	// Create damage collision sphere
	DamageCollision = CreateDefaultSubobject<USphereComponent>(TEXT("DamageCollision"));
	DamageCollision->SetupAttachment(ShieldMesh);
	DamageCollision->SetSphereRadius(50.0f);
	DamageCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DamageCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	DamageCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	DamageCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
    
	// Create shield effect (optional Niagara effect)
	ShieldEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ShieldEffect"));
	ShieldEffect->SetupAttachment(ShieldMesh);
    
	// Initialize variables
	OwnerCharacter = nullptr;
	CurrentAngle = 0.0f;
}

// Called when the game starts or when spawned
void ARotatingShield::BeginPlay()
{
	Super::BeginPlay();
	// Bind overlap event
	if (DamageCollision)
	{
		DamageCollision->OnComponentBeginOverlap.AddDynamic(this, &ARotatingShield::OnShieldBeginOverlap);
	}
    
	UE_LOG(LogTemp, Warning, TEXT("RotatingShield BeginPlay - OwnerCharacter is %s"), 
		OwnerCharacter ? TEXT("Valid") : TEXT("NULL"));
}

// Called every frame
void ARotatingShield::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Always update position if we have an owner
	if (OwnerCharacter && IsValid(OwnerCharacter))
	{
		UpdateShieldPosition(DeltaTime);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("RotatingShield Tick - No valid OwnerCharacter!"));
	}
}

void ARotatingShield::InitializeShield(AThegameCharacter* OwningCharacter)
{
	OwnerCharacter = OwningCharacter;
    
	if (OwnerCharacter)
	{
		// Set initial position immediately
		CurrentAngle = 0.0f;
		UpdateShieldPosition(0.0f);
        
		UE_LOG(LogTemp, Warning, TEXT("Rotating Shield initialized for player: %s"), 
			*OwnerCharacter->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("InitializeShield called with NULL OwningCharacter!"));
	}
}

void ARotatingShield::OnShieldBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Ignore self and owner
	if (!OtherActor || OtherActor == this || OtherActor == OwnerCharacter)
		return;
    
	// Check if we can damage this enemy (cooldown check)
	if (!CanDamageEnemy(OtherActor))
		return;
    
	// Check if it's a regular enemy
	AEnemy* Enemy = Cast<AEnemy>(OtherActor);
	if (Enemy)
	{
		// Apply damage
		UGameplayStatics::ApplyDamage(Enemy, ShieldDamage, nullptr, this, nullptr);
        
		// Apply stun effect
		ApplyStunToEnemy(Enemy);
        
		// Update damage timestamp
		DamagedEnemiesTimestamp.Add(OtherActor, GetWorld()->GetTimeSeconds());
        
		// Visual feedback
		FString DamageMessage = FString::Printf(TEXT("Shield hit enemy for %.0f damage!"), ShieldDamage);
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, DamageMessage);
        
		UE_LOG(LogTemp, Warning, TEXT("Rotating Shield hit enemy: %s"), *Enemy->GetName());
		return;
	}
    
	// Check if it's a boss
	ABossTest* Boss = Cast<ABossTest>(OtherActor);
	if (Boss)
	{
		// Apply damage to boss (no stun)
		Boss->DealDamage(ShieldDamage);
        
		// Update damage timestamp
		DamagedEnemiesTimestamp.Add(OtherActor, GetWorld()->GetTimeSeconds());
        
		// Visual feedback
		FString DamageMessage = FString::Printf(TEXT("Shield hit boss for %.0f damage!"), ShieldDamage);
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, DamageMessage);
        
		UE_LOG(LogTemp, Warning, TEXT("Rotating Shield hit boss"));
	}
}

void ARotatingShield::UpdateShieldPosition(float DeltaTime)
{
	if (!OwnerCharacter || !IsValid(OwnerCharacter))
    {
        UE_LOG(LogTemp, Error, TEXT("UpdateShieldPosition - OwnerCharacter is invalid!"));
        return;
    }
    
    // Update rotation angle for CLOCKWISE rotation (when viewed from above)
    CurrentAngle -= RotationSpeed * DeltaTime;
    
    // Wrap angle to keep it between 0 and 360
    if (CurrentAngle < 0.0f)
    {
        CurrentAngle += 360.0f;
    }
    
    // Get owner's current location
    FVector OwnerLocation = OwnerCharacter->GetActorLocation();
    
    // Calculate new position
    float RadianAngle = FMath::DegreesToRadians(CurrentAngle);
    
    // Add height offset to position shield at character's midsection
    float HeightOffset = 50.0f; // Adjust this value to position shield at desired height
    
    // Calculate offset position for clockwise rotation
    float X = FMath::Cos(RadianAngle) * OrbitRadius;
    float Y = FMath::Sin(RadianAngle) * OrbitRadius;
    
    FVector NewLocation = OwnerLocation + FVector(X, Y, HeightOffset);
    
    // Set the new location
    SetActorLocation(NewLocation);
    
    // Make shield face outward from center
    FVector DirectionFromOwner = (NewLocation - OwnerLocation).GetSafeNormal();
    FRotator NewRotation = DirectionFromOwner.Rotation();
	
    SetActorRotation(NewRotation);
	
}

bool ARotatingShield::CanDamageEnemy(AActor* Enemy)
{
	if (!Enemy)
		return false;
    
	// Check if enemy is in our damage map
	float* LastDamageTime = DamagedEnemiesTimestamp.Find(Enemy);
	if (!LastDamageTime)
	{
		// First time hitting this enemy
		return true;
	}
    
	// Check if enough time has passed since last damage
	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - *LastDamageTime >= DamageInterval)
	{
		return true;
	}
    
	return false;
}

void ARotatingShield::ApplyStunToEnemy(AEnemy* Enemy)
{
	if (!Enemy || !Enemy->GetCharacterMovement())
		return;
    
	// Store original speed
	float OriginalSpeed = Enemy->GetCharacterMovement()->MaxWalkSpeed;
    
	// Stop the enemy movement (stun)
	Enemy->GetCharacterMovement()->DisableMovement();
    
	// Create a timer to re-enable movement after stun duration
	FTimerHandle StunTimerHandle;
	FTimerDelegate StunDelegate;
	StunDelegate.BindLambda([Enemy, OriginalSpeed]()
	{
		if (IsValid(Enemy) && Enemy->GetCharacterMovement())
		{
			Enemy->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			Enemy->GetCharacterMovement()->MaxWalkSpeed = OriginalSpeed;
            
			UE_LOG(LogTemp, Warning, TEXT("Enemy stun ended"));
		}
	});
    
	GetWorld()->GetTimerManager().SetTimer(StunTimerHandle, StunDelegate, StunDuration, false);
    
	UE_LOG(LogTemp, Warning, TEXT("Enemy stunned for %.1f seconds"), StunDuration);
}

