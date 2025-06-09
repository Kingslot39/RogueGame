// Fill out your copyright notice in the Description page of Project Settings.


#include "HitStop.h"

// Sets default values
AHitStop::AHitStop()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHitStop::BeginPlay()
{
	Super::BeginPlay();
	UWorld* W = GetWorld();
	if (!W) { Destroy(); return; }

	OriginalDilation = W->GetWorldSettings()->GetEffectiveTimeDilation();

	W->GetWorldSettings()->SetTimeDilation(SlowMoFactor);
	W->GetTimerManager().SetTimer(
		TimerHandle_EndHitStop, this, &AHitStop::EndHitStop, Duration, false
	);
}

// Called every frame
void AHitStop::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHitStop::EndHitStop()
{
	if (UWorld* W = GetWorld())
	{
		W->GetWorldSettings()->SetTimeDilation(OriginalDilation);
	}
	Destroy();
}

