// Fill out your copyright notice in the Description page of Project Settings.


#include "BossTutorialAIController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

ABossTutorialAIController::ABossTutorialAIController()
{
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard Component"));
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTree Component"));

	
}

void ABossTutorialAIController::BeginPlay()
{
	Super::BeginPlay();
	RunBehaviorTree(BehaviourTree);
	BehaviorTreeComponent->StartTree(*BehaviourTree);
}

void ABossTutorialAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if(BlackboardComponent){
	  Blackboard->InitializeBlackboard(*BehaviourTree->BlackboardAsset);	
	}
}
