// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_RandomSKillPick.generated.h"

/**
 * 
 */
UCLASS()
class THEGAME_API UBTT_RandomSKillPick : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTT_RandomSKillPick();
	struct FBlackboardKeySelector SkillIndexKey;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
