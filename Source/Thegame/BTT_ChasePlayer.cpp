// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_ChasePlayer.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "ThegameCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_ChasePlayer::UBTT_ChasePlayer(FObjectInitializer const& ObjectInit)
{
	NodeName = TEXT("Chase Player");
}

EBTNodeResult::Type UBTT_ChasePlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if(NavSystem)
	{
		UBlackboardComponent* BlackboardComponent = Cast<UBlackboardComponent>(OwnerComp.GetAIOwner()->GetComponentByClass(UBlackboardComponent::StaticClass()));

		FVector Location = BlackboardComponent->GetValueAsVector(GetSelectedBlackboardKey());
		AThegameCharacter* Player = Cast<AThegameCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
		OwnerComp.GetAIOwner()->MoveToActor(Player, 150);
		

		
	}
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

	return EBTNodeResult::Succeeded;

}
