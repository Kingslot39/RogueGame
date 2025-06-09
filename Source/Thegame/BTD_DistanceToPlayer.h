// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "BTD_DistanceToPlayer.generated.h"

/**
 * 
 */
UCLASS()
class THEGAME_API UBTD_DistanceToPlayer : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTD_DistanceToPlayer();

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float FarDistance = 500.0f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float NearDistance = 100.0f;
protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	
};
