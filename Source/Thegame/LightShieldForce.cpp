// Fill out your copyright notice in the Description page of Project Settings.


#include "LightShieldForce.h"

#include "Enemy.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ALightShieldForce::ALightShieldForce()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	LightSkillMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Light Arrow Mesh"));
	LightSkillMesh->SetupAttachment(RootComponent);
	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement Component"));

	MovementComponent->bShouldBounce = false;
	MovementComponent->BounceVelocityStopSimulatingThreshold = MovementSpeed / 2;
	MovementComponent->bSweepCollision = true;
	MovementComponent->InitialSpeed = MovementSpeed;
	MovementComponent->UpdatedComponent = LightSkillMesh;
	MovementComponent->ProjectileGravityScale = 0;


}

void ALightShieldForce::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector Normal, const FHitResult& Hit)
{
	AEnemy* HitEnemy = Cast<AEnemy>(OtherActor);
	if(HitEnemy)
	{
		HitEnemy->GetDamage(SkillDamage);
		FVector SpawnLocation = HitEnemy->GetActorLocation() - FVector(0,0,100);
		GetWorld()->SpawnActor<AActor>(ShieldForceArea,SpawnLocation, FRotator::ZeroRotator);
		Destroy();
	}
}

// Called when the game starts or when spawned
void ALightShieldForce::BeginPlay()
{
	Super::BeginPlay();
	LightSkillMesh->OnComponentHit.AddDynamic(this, &ALightShieldForce::OnHit);
	LightSkillMesh->SetNotifyRigidBodyCollision(true);
	
}

// Called every frame
void ALightShieldForce::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

