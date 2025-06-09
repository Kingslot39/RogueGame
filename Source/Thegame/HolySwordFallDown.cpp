// Fill out your copyright notice in the Description page of Project Settings.


#include "HolySwordFallDown.h"



void AHolySwordFallDown::DestroySword()
{
	Destroy();
}

void AHolySwordFallDown::PlaySwordFallDown()
{
	
	GetWorldTimerManager().SetTimer(DelayTime, this, &AHolySwordFallDown::FadingOutSword, 1.0f, false);
}

void AHolySwordFallDown::FadingOutSword()
{
	if(SwordMesh)
	{
		UMaterialInterface* Material = SwordMesh->GetMaterial(0);
		if(Material)
		{
			DynamicMaterial = SwordMesh->CreateAndSetMaterialInstanceDynamic(0);
		}
	}
	GetWorldTimerManager().SetTimer(DestroyTime, this, &AHolySwordFallDown::DestroySword, 1.4f, false);
}

// Sets default values
AHolySwordFallDown::AHolySwordFallDown()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SwordMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Sword Fall Down"));
	
	RootComponent = SwordMesh;

}



// Called when the game starts or when spawned
void AHolySwordFallDown::BeginPlay()
{
	Super::BeginPlay();
	
	SwordMesh->SetWorldScale3D(FVector(13,13,13));
	
	
}

// Called every frame
void AHolySwordFallDown::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(DynamicMaterial)
	{
		Progress += DeltaTime;
		float AnimatedValue = FMath::Sin(Progress) * 0.5f + 0.5f;
		DynamicMaterial->SetScalarParameterValue(FName("Progress"),AnimatedValue);
	}
}

