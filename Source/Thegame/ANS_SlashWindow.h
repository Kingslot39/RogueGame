// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "BossCombatComponent.h"
#include "ANS_SlashWindow.generated.h"

/**
 * 
 */
UCLASS()
class THEGAME_API UANS_SlashWindow : public UAnimNotifyState
{
	GENERATED_BODY()
private:
	FVector SlashBegin;
	FVector SlashEnd;

	float TimeBetweenCheck = 0.005f;
	float CheckTime = 0.005f;
public:
	UPROPERTY(EditAnywhere)
	FString SocketName;

	UPROPERTY(EditAnywhere)
	float fRadius;

	UPROPERTY(EditAnywhere)
	FAttackStats AttackStats;

	UPROPERTY(EditAnywhere)
	bool ContinuousTracing = true;

	UPROPERTY(EditAnywhere)
	int NumTraceChecks = 5;

	UPROPERTY(EditAnywhere)
	bool DebugMode = false;

	UFUNCTION()
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	UFUNCTION()
	virtual  void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	UFUNCTION()
	virtual  void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
};
