// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_ScytheSlash.generated.h"

/**
 * 
 */
UCLASS()
class THEGAME_API UBTT_ScytheSlash : public UBTTaskNode
{
	GENERATED_BODY()
	UBTT_ScytheSlash();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
