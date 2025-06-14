// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_BossChasePlayer.generated.h"

/**
 * 
 */
UCLASS()
class THEGAME_API UBTT_BossChasePlayer : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTT_BossChasePlayer(FObjectInitializer const& ProjectInit);
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
