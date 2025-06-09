// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootTask.h"

#include "BossAIController.h"

EBTNodeResult::Type UShootTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
	BTController->Shoot();
	return EBTNodeResult::Succeeded;

}
