// Fill out your copyright notice in the Description page of Project Settings.


#include "BTD_Wait.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTD_Wait::UBTD_Wait()
{
	NodeName = "Delay Skill Time";
}

bool UBTD_Wait::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if(!BlackboardComponent)
	{
		return false;
	}
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	const float StoreTime = BlackboardComponent->GetValueAsFloat(TimeStampKey.SelectedKeyName);
	if(CurrentTime - StoreTime >= DelayDuration)
	{
		BlackboardComponent->SetValueAsFloat(TimeStampKey.SelectedKeyName, CurrentTime);
		return true;
	}
	return  false;

}
