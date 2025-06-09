// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_SpawnTree.generated.h"

/**
 * 
 */
UCLASS()
class THEGAME_API UBTT_SpawnTree : public UBTTaskNode
{
	GENERATED_BODY()
	UBTT_SpawnTree();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
