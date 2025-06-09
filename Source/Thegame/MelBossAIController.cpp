// Fill out your copyright notice in the Description page of Project Settings.


#include "MelBossAIController.h"

#include "MelBoss.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

AMelBossAIController::AMelBossAIController()
{
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard Component"));
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTree Component"));
}

void AMelBossAIController::BeginPlay()
{
	Super::BeginPlay();
	TargetPlayer = Cast<AThegameCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	RunBehaviorTree(BehaviourTree);
	BehaviorTreeComponent->StartTree(*BehaviourTree);
}

void AMelBossAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if(BlackboardComponent){
		Blackboard->InitializeBlackboard(*BehaviourTree->BlackboardAsset);	
	}
}

UBlackboardComponent* AMelBossAIController::GetBlackBoard() const
{
	return BlackboardComponent;
}

void AMelBossAIController::UmbrellaBloom()
{
	AMelBoss* MelBoss = Cast<AMelBoss>(GetPawn());
	MelBoss->UmbrellaBloomTelegraph();
}

void AMelBossAIController::SwordRush()
{
	AMelBoss* MelBoss = Cast<AMelBoss>(GetPawn());
	MelBoss->JudmentSwordRush();
}

void AMelBossAIController::ScytheSlash()
{
	AMelBoss* MelBoss = Cast<AMelBoss>(GetPawn());
	if(MelBoss->CurrentHealth < 1500)
	{
		MelBoss->ScytheSlash();
	}
	
}

void AMelBossAIController::HammerSlash()
{
	AMelBoss* MelBoss = Cast<AMelBoss>(GetPawn());
	
	if(MelBoss->CurrentHealth >= 1500)
	{
		MelBoss->SlashHammer();
	}
	else
	{
		MelBoss->GodSlamDown();
	}
}

