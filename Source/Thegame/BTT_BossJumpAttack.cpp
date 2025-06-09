// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_BossJumpAttack.h"

#include "AIController.h"
#include "BossAIController.h"
#include "BossCombatComponent.h"
#include "BossTest.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_BossJumpAttack::UBTT_BossJumpAttack()
{
	NodeName = TEXT("Summon Sword");
}

EBTNodeResult::Type UBTT_BossJumpAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
	BTController->SummonSword();
	return EBTNodeResult::Succeeded;

	
}
