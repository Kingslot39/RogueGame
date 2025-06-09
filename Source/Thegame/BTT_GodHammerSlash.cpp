// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_GodHammerSlash.h"

#include "MelBossAIController.h"

UBTT_GodHammerSlash::UBTT_GodHammerSlash()
{
	  NodeName = "Hammer Slash";
}

EBTNodeResult::Type UBTT_GodHammerSlash::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
	BTController->HammerSlash();
	return EBTNodeResult::Succeeded;
}
