// LightShieldForce.cpp

#include "LightShieldForce.h"
#include "Enemy.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ALightShieldForce::ALightShieldForce()
{
	//Default Movement
	MovementSpeed = 3000;
	// Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	LightSkillMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Light Arrow Mesh"));
	LightSkillMesh->SetupAttachment(RootComponent);
	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement Component"));

	MovementComponent->bShouldBounce = false;
	MovementComponent->BounceVelocityStopSimulatingThreshold = MovementSpeed / 2;
	MovementComponent->bSweepCollision = true;
	MovementComponent->InitialSpeed = MovementSpeed / 2;  // Slower by larger amount
	MovementComponent->MaxSpeed = MovementSpeed /2;	  // Slower by larger amount
	MovementComponent->UpdatedComponent = LightSkillMesh;
	MovementComponent->ProjectileGravityScale = 0;
}

void ALightShieldForce::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector Normal, const FHitResult& Hit)
{
	AEnemy* HitEnemy = Cast<AEnemy>(OtherActor);
	if (HitEnemy)
	{
		// It hit an enemy
		HitEnemy->GetDamage(SkillDamage);
		FVector SpawnLocation = HitEnemy->GetActorLocation() - FVector(0, 0, 100);
		GetWorld()->SpawnActor<AActor>(ShieldForceArea, SpawnLocation, FRotator::ZeroRotator);
	}

	// Destroy the projectile in all cases (enemy or non-enemy hit)
	Destroy();
}

// Called when the game starts or when spawned
void ALightShieldForce::BeginPlay()
{
	Super::BeginPlay();

	// Ensure the OnHit event is only bound once
	LightSkillMesh->OnComponentHit.AddDynamic(this, &ALightShieldForce::OnHit);
	LightSkillMesh->SetNotifyRigidBodyCollision(true);
}

// Called every frame
void ALightShieldForce::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
