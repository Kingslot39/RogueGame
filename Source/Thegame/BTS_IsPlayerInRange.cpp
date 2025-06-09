// Fill out your copyright notice in the Description page of Project Settings.


#include "BTS_IsPlayerInRange.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

UBTS_IsPlayerInRange::UBTS_IsPlayerInRange()
{
	bNotifyBecomeRelevant = true;
	NodeName = TEXT("Is Player In Range");
}

void UBTS_IsPlayerInRange::OnBecomeRelevant(UBehaviorTreeComponent& TreeComponent, uint8* NodeMem)
{
	Super::OnBecomeRelevant(TreeComponent, NodeMem);
	UBlackboardComponent* BlackboardComponent = Cast<UBlackboardComponent>(TreeComponent.GetAIOwner()->GetComponentByClass(UBlackboardComponent::StaticClass()));
	BlackboardComponent->SetValueAsBool(GetSelectedBlackboardKey(),TreeComponent.GetAIOwner()->GetPawn()->GetDistanceTo(Cast<AActor>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0)))< fRange );
	
	
	
}
