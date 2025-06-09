// Fill out your copyright notice in the Description page of Project Settings.


#include "UmbrellaBloom.h"

// Sets default values
AUmbrellaBloom::AUmbrellaBloom()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	UmbrellaMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Umbrella Mesh"));
	RootComponent = UmbrellaMesh;

}

void AUmbrellaBloom::SkillRun()
{
	GetWorldTimerManager().SetTimer(DestroyTime, this, &AUmbrellaBloom::DestroyUmbrella, 3.0f, false);
}

void AUmbrellaBloom::DestroyUmbrella()
{
	Destroy();
}

// Called when the game starts or when spawned
void AUmbrellaBloom::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AUmbrellaBloom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

