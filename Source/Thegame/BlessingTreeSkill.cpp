// Fill out your copyright notice in the Description page of Project Settings.


#include "BlessingTreeSkill.h"

// Sets default values
ABlessingTreeSkill::ABlessingTreeSkill()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ABlessingTreeSkill::SpawnTree()
{
	isSkillActivate = true;
	GetWorldTimerManager().SetTimer(RemainTime, this, &ABlessingTreeSkill::DestroyTree, 5.0f, false);
}

void ABlessingTreeSkill::DestroyTree()
{
	isSkillActivate = false;
	Destroy();
}

// Called when the game starts or when spawned
void ABlessingTreeSkill::BeginPlay()
{
	Super::BeginPlay();
	Target = Cast<AThegameCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	
}

// Called every frame
void ABlessingTreeSkill::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(isSkillActivate)
	{
		Target->HealthPlayer(1);
	}

}

