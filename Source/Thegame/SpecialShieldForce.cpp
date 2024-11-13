// Fill out your copyright notice in the Description page of Project Settings.


#include "SpecialShieldForce.h"

#include "Enemy.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ASpecialShieldForce::ASpecialShieldForce()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	TrapArea = CreateDefaultSubobject<UBoxComponent>(TEXT("TrapArea"));
	TrapArea->SetCollisionProfileName("Trigger");
	RootComponent = TrapArea;

	TrapArea->OnComponentBeginOverlap.AddDynamic(this, &ASpecialShieldForce::OnOverlapBegin);

}

void ASpecialShieldForce::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AEnemy* TrappedEnemy = Cast<AEnemy>(OtherActor);
	if(TrappedEnemy)
	{
		TrappedEnemy->GetCharacterMovement()->SetMovementMode(MOVE_None);
		GetWorld()->GetTimerManager().SetTimer(CoolDownTime, this, &ASpecialShieldForce::AreaDestroy, 5.f, false);
	}
}

void ASpecialShieldForce::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AEnemy* TrappedActor = Cast<AEnemy>(OtherActor);
	if(TrappedActor)
	{
		TrappedActor->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}

void ASpecialShieldForce::AreaDestroy()
{
	Destroy();
}

// Called when the game starts or when spawned
void ASpecialShieldForce::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpecialShieldForce::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

