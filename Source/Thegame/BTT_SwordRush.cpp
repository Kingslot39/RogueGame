// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_SwordRush.h"

#include "MelBossAIController.h"

UBTT_SwordRush::UBTT_SwordRush()
{
	NodeName = "Sword Rush";
}

EBTNodeResult::Type UBTT_SwordRush::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBehaviorTreeComponent* BTComp = &OwnerComp;
	if(!BTComp)
	{
		return EBTNodeResult::Failed;
	}
	AMelBossAIController* BTController = Cast<AMelBossAIController>(BTComp->GetOwner());
	if(!BTController)
	{
		return EBTNodeResult::Failed;
	}
	BTController->SwordRush();
	return EBTNodeResult::Succeeded;
}
