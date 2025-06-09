// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_JumpCurve.h"

#include "Curves/CurveVector.h"

void UANS_JumpCurve::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
                                 const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	Duration = TotalDuration;
	
}

void UANS_JumpCurve::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	FVector MeshLoc = MeshComp->GetOwner()->GetActorLocation();
	FVector CurveVec = Curve->GetVectorValue(FrameDeltaTime/Duration);
	FRotator MeshRot = MeshComp->GetOwner()->GetActorRotation();

	FVector fUpward = MeshComp->GetOwner()->GetActorUpVector() * CurveVec.Z * Height;
	FVector fFoward = MeshComp->GetOwner()->GetActorForwardVector() * CurveVec.X * Foward;
	FVector fRightward = MeshComp->GetOwner()->GetActorRightVector() * CurveVec.Y * Rightward;

	FVector CurveLoc = fUpward + fFoward + fRightward;

	MeshComp->GetOwner()->TeleportTo(MeshLoc + CurveLoc, MeshRot);
	
}
