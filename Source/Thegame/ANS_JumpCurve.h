// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Curves/CurveVector.h"
#include "ANS_JumpCurve.generated.h"

/**
 * 
 */
UCLASS()
class THEGAME_API UANS_JumpCurve : public UAnimNotifyState
{
	GENERATED_BODY()
private:
	float Duration;
public:
	UPROPERTY(EditAnywhere)
	UCurveVector* Curve;

	UPROPERTY(EditAnywhere)
	float Height;

	UPROPERTY(EditAnywhere)
	float Foward;

	UPROPERTY(EditAnywhere)
	float Rightward;

	UFUNCTION()
	virtual  void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	UFUNCTION()
	virtual  void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	
	
};
