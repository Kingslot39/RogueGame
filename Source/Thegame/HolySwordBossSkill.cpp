// Fill out your copyright notice in the Description page of Project Settings.


#include "HolySwordBossSkill.h"

// Sets default values
AHolySwordBossSkill::AHolySwordBossSkill()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SwordMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Sword Mesh"));
	

}



void AHolySwordBossSkill::SlashDown()
{
	GetWorldTimerManager().SetTimer(SlashTimeHandle, this, &AHolySwordBossSkill::DestroySword, 3.1f, false);
	GetWorldTimerManager().SetTimer(WaitingToFadeTime, this, &AHolySwordBossSkill::FadingOut, 2.0f, false);
}


void AHolySwordBossSkill::FadingOut()
{
	if(SwordMesh)
	{
		UMaterialInterface* Material = SwordMesh->GetMaterial(0);
		if(Material)
		{
			 DynamicMaterial = SwordMesh->CreateAndSetMaterialInstanceDynamic(0);
			
		}
	}
	
}

void AHolySwordBossSkill::DestroySword()
{
	Destroy();
}

// Called when the game starts or when spawned
void AHolySwordBossSkill::BeginPlay()
{
	Super::BeginPlay();
   
}

// Called every frame
void AHolySwordBossSkill::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    if(DynamicMaterial)
    {
	    Progress += DeltaTime;
    	float AnimatedValue = FMath::Sin(Progress) * 0.5f + 0.5f;
    	DynamicMaterial->SetScalarParameterValue(FName("Progress"),AnimatedValue);
    }
}

