// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_BossChasePlayer.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTT_BossChasePlayer::UBTT_BossChasePlayer(FObjectInitializer const& ProjectInit)
{
	NodeName = TEXT("ChasePlayer");
}

EBTNodeResult::Type UBTT_BossChasePlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());

	if(NavSystem)
	{
		UBlackboardComponent* BlackboardComponent = Cast<UBlackboardComponent>(OwnerComp.GetAIOwner()->GetComponentByClass(UBlackboardComponent::StaticClass()));

		FVector Location = BlackboardComponent->GetValueAsVector(GetSelectedBlackboardKey());

		OwnerComp.GetAIOwner()->MoveToLocation(Location);
	}
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

	return EBTNodeResult::Succeeded;
}
