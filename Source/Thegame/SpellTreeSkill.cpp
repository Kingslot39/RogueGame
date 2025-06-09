// Fill out your copyright notice in the Description page of Project Settings.


#include "SpellTreeSkill.h"

#include "BossTest.h"
#include "Enemy.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASpellTreeSkill::ASpellTreeSkill()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SpellTreeMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Tree Mesh"));
	RootComponent = SpellTreeMesh;

	ShootingDelay = 5;
	bCanShoot = true;
	FieldOfView = 45;
	AttackRange = 1000;

}

void ASpellTreeSkill::Shooting()
{
}

// Called when the game starts or when spawned
void ASpellTreeSkill::BeginPlay()
{
	Super::BeginPlay();
	Target = UGameplayStatics::GetActorOfClass(GetWorld(),AEnemy::StaticClass());
	
}

// Called every frame
void ASpellTreeSkill::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(!bCanShoot)
	{
		ReloadTime += DeltaTime;
		if(ReloadTime >= ShootingDelay)
		{
			bCanShoot = true;
			ReloadTime = 0;
		}
	}
	if(!Target)
	{
		return;
	}
	if(bCanShoot && FVector::Distance(GetActorLocation(), Target->GetActorLocation()) <= AttackRange)
	{
		const FVector Location = GetActorLocation();
		FVector Direction = Target->GetActorLocation() - Location;
		const FVector Foward = GetActorForwardVector();
		const float RotationAngleRad = FMath::Acos(Foward.Dot(Direction));
		const float RotationAngleDeg = FMath::RadiansToDegrees(RotationAngleRad);
		if(RotationAngleDeg <= FieldOfView)
		{
			Direction.Normalize();
			const FRotator ProjectileRot = Direction.Rotation();
			ABossSpell* Spell = Cast<ABossSpell>(GetWorld()->SpawnActor(SpellClass, &Location, &ProjectileRot));
			bCanShoot = false;
			
		}
	}
	

}

