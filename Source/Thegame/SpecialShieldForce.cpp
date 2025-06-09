// Fill out your copyright notice in the Description page of Project Settings.


#include "SpecialShieldForce.h"

#include "Enemy.h"


// Sets default values
ASpecialShieldForce::ASpecialShieldForce()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	TrapArea = CreateDefaultSubobject<UBoxComponent>(TEXT("TrapArea"));
	TrapArea->SetCollisionProfileName("Trigger");
	RootComponent = TrapArea;

	

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

