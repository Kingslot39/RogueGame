// Fill out your copyright notice in the Description page of Project Settings.


#include "JudgmentSwordSlice.h"

// Sets default values

AJudgmentSwordSlice::AJudgmentSwordSlice()
{
	SwordMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SwordMesh"));
	RootComponent = SwordMesh;
}



void AJudgmentSwordSlice::BeginPlay()
{
	Super::BeginPlay();
	
}

void AJudgmentSwordSlice::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
