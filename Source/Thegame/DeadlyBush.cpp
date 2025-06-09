// Fill out your copyright notice in the Description page of Project Settings.


#include "DeadlyBush.h"

// Sets default values
ADeadlyBush::ADeadlyBush()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	BushMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Bush Mesh"));
	DamageArea = CreateDefaultSubobject<UBoxComponent>(TEXT("Damage Area"));
	RootComponent = BushMesh;
	DamageArea->SetupAttachment(BushMesh);

}

void ADeadlyBush::WaitForDestroy()
{
	GetWorldTimerManager().SetTimer(DestroyWaitTime, this, &ADeadlyBush::DestroyBush, 1.5f, false);
}

void ADeadlyBush::DestroyBush()
{
	Destroy();
}

// Called when the game starts or when spawned
void ADeadlyBush::BeginPlay()
{
	Super::BeginPlay();
	BushMesh->SetWorldScale3D(FVector(15.0f,15.0f,15.0f));
	
}

// Called every frame
void ADeadlyBush::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

