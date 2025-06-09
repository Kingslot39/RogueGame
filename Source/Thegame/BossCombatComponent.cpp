#include "BossCombatComponent.h"


#include "ThegameCharacter.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"

#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UBossCombatComponent::UBossCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBossCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UBossCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UBossCombatComponent::Attack(FName AttackName)
{
	if(AttackMap.Num() == 0)
	{
		return;
	}
	UAnimInstance* Animation = Cast<ACharacter>(GetOwner())->GetMesh()->GetAnimInstance();

	if(!Animation)
	{
		return;
	}
	FAttack* Attack = AttackMap.Find(AttackName);

	if(!Attack)
	{
		return;
	}

	UAnimMontage* AnimMontage = Attack->AttackMontage;

	if(AnimMontage)
	{
		Animation->Montage_Play(AnimMontage);
	}
}

void UBossCombatComponent::StopAttack(float BlendedOutTime, FName AttackName)
{
	if(AttackMap.Num() == 0)
	{
		return;
	}
	UAnimInstance* Animation = Cast<ACharacter>(GetOwner())->GetMesh()->GetAnimInstance();

	if(!Animation)
	{
		return;
	}
	FAttack* Attack = AttackMap.Find(AttackName);

	if(!Attack)
	{
		return;
	}

	UAnimMontage* AnimMontage = Attack->AttackMontage;

	if(AnimMontage)
	{
		Animation->Montage_Play(AnimMontage);
	}
}

bool UBossCombatComponent::isAttacking()
{
	return true;
}



void UBossCombatComponent::GenerateHitSphere(FVector Location, float Radius, FAttackStats Attack, bool Debug)
{
	//Init hit results
	TArray<FHitResult> outHits;
	TArray<AActor*> ignoredActors;
	ignoredActors.Add(GetOwner());

	//Use Sphere Shape;
	FCollisionShape sphereShape;
	sphereShape.ShapeType = ECollisionShape::Sphere;
	sphereShape.SetSphere(Radius);

	//Set the query params
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());

	GetWorld()->SweepMultiByChannel(outHits, Location, Location,FQuat::Identity,ECC_Pawn ,sphereShape);

	//Debug Trace to show visual
	if(Debug)
	{
		UKismetSystemLibrary::SphereTraceMulti(GetWorld(), Location, Location, Radius, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Pawn), true, ignoredActors, EDrawDebugTrace::ForDuration, outHits, true, FColor::Red);
	}
	//Check that a collision happened
	if(outHits.Num() >0)
	{
		for(auto i = outHits.CreateIterator(); i; i++)
		{
			if(DamageActor.Contains(i->GetActor()))
			{
				continue;
			}
			else
			{
				DamageActor.Add(i->GetActor());
			}
			ACharacter* pVictim = Cast<ACharacter>(i->GetActor());
			if(pVictim)
			{
				HandleDamage(pVictim, Location, Attack);
			}
		}
	}
}

void UBossCombatComponent::GenerateHitCapsule(FVector BeginLoc, FVector EndLoc, float fRadius, FAttackStats AttackStats,
	bool Debug)
{
	TArray<FHitResult> outHits;
	TArray<AActor*> ignoredActors;
	ignoredActors.Add(GetOwner());
	
	FCollisionShape sphereShape;
	sphereShape.ShapeType = ECollisionShape::Sphere;
	sphereShape.SetSphere(fRadius);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());

	
		GetWorld()->SweepMultiByChannel(outHits, BeginLoc, EndLoc, FQuat::Identity, ECC_Pawn, sphereShape, QueryParams );
	

	if(Debug)
	{
		UKismetSystemLibrary::SphereTraceMulti(GetWorld(), BeginLoc, EndLoc, fRadius, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Pawn), true,ignoredActors ,EDrawDebugTrace::ForDuration, outHits, true, FColor::Red);
	}
	if(outHits.Num()>0)
	{
		AThegameCharacter* Player = Cast<AThegameCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
		Player->GetDamage(AttackStats.Damage);
	}
	//if(outHits.Num() >0)
	//{
	//	for(auto i = outHits.CreateIterator(); i; i++)
	//	{
	//		if(DamageActor.Contains(i->GetActor()))
	//		{
	//			continue;
	//		}
	//		else
	//		{
	//			DamageActor.Add(i->GetActor());
	//		}
	//		ACharacter* pVictim = Cast<ACharacter>(i->GetActor());
	//		if(pVictim)
	//		{
	//			HandleDamage(pVictim, BeginLoc, AttackStats);
	//		}
	//	}
	//}
}


void UBossCombatComponent::HandleDamage(ACharacter* Victim, FVector Location, FAttackStats Attack)
{
	UBossCombatComponent* VicCombat = Cast<UBossCombatComponent>(Victim->GetComponentByClass(UBossCombatComponent::StaticClass()));

	if(!VicCombat)
	{
		return;
	}
	if(VicCombat ->Health <=0)
	{
		return;
	}
	UAnimInstance* AnimInstance = Cast<ACharacter>(VicCombat->GetOwner())->GetMesh()->GetAnimInstance();

	if(!AnimInstance)
	{
		return;
	}
	if(VicCombat->HurtMontage)
	{
		AnimInstance->Montage_Play(VicCombat->HurtMontage);
	}
	VicCombat->TakeDamage(Attack.Damage);
	
}

void UBossCombatComponent::TakeDamage(float Damage)
{
	Health -= Damage;
}




	



