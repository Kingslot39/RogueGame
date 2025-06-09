// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_SpawnBush.h"

#include "BossAIController.h"

UBTT_SpawnBush::UBTT_SpawnBush()
{
	NodeName = "Spawn Bush";
}

EBTNodeResult::Type UBTT_SpawnBush::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBehaviorTreeComponent* BTComp = &OwnerComp;
	if(!BTComp)
	{
		return EBTNodeResult::Failed;
	}
	ABossAIController* BTController = Cast<ABossAIController>(BTComp->GetOwner());
	if(!BTController)
	{
		return EBTNodeResult::Failed;
	}
	BTController->SpawnBush();
	return EBTNodeResult::Succeeded;
}

