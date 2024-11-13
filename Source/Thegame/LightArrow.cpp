// Fill out your copyright notice in the Description page of Project Settings.


#include "LightArrow.h"
#include "NiagaraComponent.h"
#include "Enemy.h"
#include "NavigationSystemTypes.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ALightArrow::ALightArrow()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	LightArrowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Light Arrow Mesh"));
	LightArrowMesh->SetupAttachment(RootComponent);
	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement Component"));

	MovementComponent->bShouldBounce = false;
	MovementComponent->BounceVelocityStopSimulatingThreshold = MovementSpeed / 2;
	MovementComponent->bSweepCollision = true;
	MovementComponent->InitialSpeed = MovementSpeed;
	MovementComponent->UpdatedComponent = LightArrowMesh;
	MovementComponent->ProjectileGravityScale = 0;
	SkillDamage = 50.0f;

	

}

void ALightArrow::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector Normal, const FHitResult& Hit)
{
	AEnemy* HitEnemy = Cast<AEnemy>(OtherActor);
	if (HitEnemy)
	{
		// Deal damage to the enemy using SkillDamage
		HitEnemy->GetDamage(SkillDamage);

		// Log the damage dealt
		UE_LOG(LogTemp, Warning, TEXT("Arrow hit enemy, dealing %f damage."), SkillDamage);
	}

	// Destroy the arrow after dealing damage
	Destroy();
}



// Called when the game starts or when spawned
void ALightArrow::BeginPlay()
{
	Super::BeginPlay();
	
	LightArrowMesh->OnComponentHit.AddDynamic(this, &ALightArrow::OnHit);
	LightArrowMesh->SetNotifyRigidBodyCollision(true);
	
}

void ALightArrow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

