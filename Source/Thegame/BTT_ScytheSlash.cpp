// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_ScytheSlash.h"

#include "MelBossAIController.h"

UBTT_ScytheSlash::UBTT_ScytheSlash()
{
	NodeName = "Scythe Slash";
}

EBTNodeResult::Type UBTT_ScytheSlash::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
	BTController->ScytheSlash();
	return EBTNodeResult::Succeeded;
}
