// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_Attack.h"

#include "AIController.h"
#include "BossCombatComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_Attack::UBTT_Attack()
{
	NodeName = TEXT("Attack");
}

EBTNodeResult::Type UBTT_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBossCombatComponent* CombatComponent = Cast<UBossCombatComponent>(OwnerComp.GetAIOwner()->GetPawn()->GetComponentByClass(UBossCombatComponent::StaticClass()));

	if(!CombatComponent)
	{
		return EBTNodeResult::Failed;
	}
	if(CombatComponent->isAttacking())
	{
		UBlackboardComponent* BlackboardComponent = Cast<UBlackboardComponent>(OwnerComp.GetAIOwner()->GetComponentByClass(UBlackboardComponent::StaticClass()));
		bCanAttack = BlackboardComponent->GetValueAsBool(GetSelectedBlackboardKey());
		if(bCanAttack)
		{
			CombatComponent->Attack(Attack);
		}
	}
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
