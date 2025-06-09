// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "BTD_Wait.generated.h"

/**
 * 
 */
UCLASS()
class THEGAME_API UBTD_Wait : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTD_Wait();
	UPROPERTY(EditAnywhere)
	struct FBlackboardKeySelector TimeStampKey;

	UPROPERTY(EditAnywhere)
	float DelayDuration = 3.0f;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	
	
};
