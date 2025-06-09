// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageArea.h"

// Sets default values
ADamageArea::ADamageArea()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	DamageArea = CreateDefaultSubobject<UBoxComponent>(TEXT("Damage Area"));
	

}

void ADamageArea::DestroyArea()
{
	Destroy();
}

void ADamageArea::WaitingTime()
{
	GetWorldTimerManager().SetTimer(DestroyAreaTime, this, &ADamageArea::DestroyArea, 1.0, false);
}

// Called when the game starts or when spawned
void ADamageArea::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADamageArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

