// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_SlashWindow.h"


void UANS_SlashWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
                                   const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	AActor* OwningActor = MeshComp->GetOwner();
	UBossCombatComponent* CombatComponent = Cast<UBossCombatComponent>(OwningActor->GetComponentByClass(UBossCombatComponent::StaticClass()));

	FVector Location = MeshComp->GetSocketLocation(FName(SocketName));

	if(!ContinuousTracing)
	{
		CombatComponent->GenerateHitSphere(Location,  fRadius, AttackStats, DebugMode);
	}
	SlashBegin = Location;
	TimeBetweenCheck = TotalDuration / static_cast<float>(NumTraceChecks);
	CheckTime = TimeBetweenCheck;
}


void UANS_SlashWindow::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	CheckTime -= FrameDeltaTime;
	if(CheckTime <= 0)
	{
		AActor* owningActor = MeshComp->GetOwner();
		UBossCombatComponent* CombatComponent = Cast<UBossCombatComponent>(owningActor->GetComponentByClass(UBossCombatComponent::StaticClass()));

		FVector Location = MeshComp->GetSocketLocation(FName(SocketName));
		SlashEnd = Location;

		if(ContinuousTracing)
		{
			CombatComponent->GenerateHitCapsule(SlashBegin, SlashEnd, fRadius, AttackStats, DebugMode);
		}
		SlashBegin = Location;
		CheckTime = TimeBetweenCheck;
	}
}

void UANS_SlashWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	AActor* owningActor = MeshComp->GetOwner();
	UBossCombatComponent* CombatComponent = Cast<UBossCombatComponent>(owningActor->GetComponentByClass(UBossCombatComponent::StaticClass()));

	FVector Location = MeshComp->GetSocketLocation(FName(SocketName));
	SlashEnd = Location;
	if(ContinuousTracing)
	{
		CombatComponent->GenerateHitCapsule(SlashBegin, SlashEnd, fRadius, AttackStats, DebugMode);
	}
	CombatComponent->DamageActor.Empty();
}
