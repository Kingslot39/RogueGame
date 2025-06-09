// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_SpawnTree.h"

#include "BossAIController.h"

UBTT_SpawnTree::UBTT_SpawnTree()
{
	NodeName = "SpawnTree";
}

EBTNodeResult::Type UBTT_SpawnTree::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
	BTController->SpawnTree();
	return EBTNodeResult::Succeeded;
}
