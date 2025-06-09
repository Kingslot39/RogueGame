// Fill out your copyright notice in the Description page of Project Settings.


#include "BossSpell.h"

#include "ThegameCharacter.h"

// Sets default values
ABossSpell::ABossSpell()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement Component"));
	MovementComponent->bShouldBounce = true;
	MovementComponent->BounceVelocityStopSimulatingThreshold = MovementSpeed / 2;
	MovementComponent->bSweepCollision = true;
	MovementComponent->InitialSpeed = MovementSpeed;
	MovementComponent->UpdatedComponent = Mesh;
	MovementComponent->ProjectileGravityScale = 0;


}

void ABossSpell::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector Normal,
	const FHitResult& Hit)
{
	if(OtherActor && OtherActor != this)
	{
		
		AThegameCharacter* HitCharacter = Cast<AThegameCharacter>(OtherActor);
		if(HitCharacter)
		{
			HitCharacter->GetDamage(10);
			Destroy();
		}
	}
}

// Called when the game starts or when spawned
void ABossSpell::BeginPlay()
{
	Super::BeginPlay();
	Mesh->OnComponentHit.AddDynamic(this, &ABossSpell::OnHit);
	Mesh->SetNotifyRigidBodyCollision(true);
	
}

// Called every frame
void ABossSpell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

