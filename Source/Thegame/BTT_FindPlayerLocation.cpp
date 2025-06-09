// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_FindPlayerLocation.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UBTT_FindPlayerLocation::UBTT_FindPlayerLocation(FObjectInitializer const& pObjectInit)
{
	NodeName = TEXT("Find Player Location");
}

EBTNodeResult::Type UBTT_FindPlayerLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(),0);
	APawn* Player = PlayerController->GetPawn();

	UBlackboardComponent* BlackboardComponent = Cast<UBlackboardComponent>(OwnerComp.GetAIOwner()->GetComponentByClass(UBlackboardComponent::StaticClass()));

	if(NavSystem)
	{
		BlackboardComponent->SetValueAsVector(GetSelectedBlackboardKey(),Player->GetActorLocation());
	}
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

	return EBTNodeResult::Succeeded;
} 
