// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_UmbrellaBloom.h"

#include "MelBossAIController.h"

UBTT_UmbrellaBloom::UBTT_UmbrellaBloom()
{
	NodeName = "Umbrella Bloom";
}

EBTNodeResult::Type UBTT_UmbrellaBloom::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
	BTController->UmbrellaBloom();
	return EBTNodeResult::Succeeded;
}
