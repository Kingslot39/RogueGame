// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_RandomSKillPick.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTT_RandomSKillPick::UBTT_RandomSKillPick()
{
	NodeName = "Pick Random Skill";
}

EBTNodeResult::Type UBTT_RandomSKillPick::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	int32 RandomSkill = FMath::RandRange(0,2);
	OwnerComp.GetBlackboardComponent()->SetValueAsInt(SkillIndexKey.SelectedKeyName, RandomSkill);
	return EBTNodeResult::Succeeded;
}
