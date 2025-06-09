// Fill out your copyright notice in the Description page of Project Settings.


#include "BushWallSkill.h"

// Sets default values
ABushWallSkill::ABushWallSkill()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	BushWallMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Bush Wall Mesh"));
	RootComponent = BushWallMesh;
	

}

void ABushWallSkill::SpawnBush()
{
	GetWorldTimerManager().SetTimer(SpawningTime, this, &ABushWallSkill::DestroyBush, 5.0f, false);
}

void ABushWallSkill::DestroyBush()
{
	Destroy();
}

// Called when the game starts or when spawned
void ABushWallSkill::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABushWallSkill::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

