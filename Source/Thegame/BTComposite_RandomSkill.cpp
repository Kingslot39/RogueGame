// Fill out your copyright notice in the Description page of Project Settings.


#include "BTComposite_RandomSkill.h"


UBTComposite_RandomSkill::UBTComposite_RandomSkill(const FObjectInitializer& ObjectInitializer)
{
	NodeName = "Random Skill Pick";
}

int32 UBTComposite_RandomSkill::GetNextChildHandler(FBehaviorTreeSearchData& SearchData, int32 PrevChild,
	EBTNodeResult::Type LastResult) const
{

	int32 NextChildIdx = BTSpecialChild::ReturnToParent;
	if(PrevChild == BTSpecialChild::NotInitialized)
	{
		NextChildIdx = FMath::RandRange(0, GetChildrenNum()-1);
	}
	else if(LastResult == EBTNodeResult::Succeeded)
	{
		NextChildIdx = FMath::RandRange(0, GetChildrenNum()-1);
	}
	return NextChildIdx;
		
	
}

void UBTComposite_RandomSkill::InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTMemoryInit::Type InitType) const
{
	
}

void UBTComposite_RandomSkill::CleanupMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTMemoryClear::Type CleanupType) const
{
	
}


