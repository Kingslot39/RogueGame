// Fill out your copyright notice in the Description page of Project Settings.


#include "HammerSlash.h"

// Sets default values
AHammerSlash::AHammerSlash()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	HammerMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Hammer Mesh"));
	RootComponent = HammerMesh;

}

// Called when the game starts or when spawned
void AHammerSlash::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHammerSlash::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

