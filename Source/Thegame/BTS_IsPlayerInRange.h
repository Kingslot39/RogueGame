// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTS_IsPlayerInRange.generated.h"

/**
 * 
 */
UCLASS()
class THEGAME_API UBTS_IsPlayerInRange : public UBTService_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTS_IsPlayerInRange();

	void OnBecomeRelevant(UBehaviorTreeComponent& TreeComponent, uint8* NodeMem);

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "AI")
	float fRange = 100.f;

	
	
};
