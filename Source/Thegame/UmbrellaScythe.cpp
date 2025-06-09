// Fill out your copyright notice in the Description page of Project Settings.


#include "UmbrellaScythe.h"

// Sets default values
AUmbrellaScythe::AUmbrellaScythe()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ScytheMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Scythe Mesh"));
	HitComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Hit Box"));
	ScytheMesh->SetupAttachment(HitComponent);
	RootComponent = ScytheMesh;

}

void AUmbrellaScythe::ScytheActivate()
{
	GetWorldTimerManager().SetTimer(ActivateTime, this, &AUmbrellaScythe::DestroyScythe, 3.0f, false);
}

void AUmbrellaScythe::DestroyScythe()
{
	Destroy();
}

// Called when the game starts or when spawned
void AUmbrellaScythe::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AUmbrellaScythe::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

