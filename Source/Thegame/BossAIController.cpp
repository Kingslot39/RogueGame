// Fill out your copyright notice in the Description page of Project Settings.


#include "BossAIController.h"

#include "BossTest.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

ABossAIController::ABossAIController()
{
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard Component"));
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTree Component"));

	
}

void ABossAIController::BeginPlay()
{
	Super::BeginPlay();
	TargetPlayer = Cast<AThegameCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	RunBehaviorTree(BehaviourTree);
	BehaviorTreeComponent->StartTree(*BehaviourTree);
}

void ABossAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if(BlackboardComponent){
		Blackboard->InitializeBlackboard(*BehaviourTree->BlackboardAsset);	
	}
}

UBlackboardComponent* ABossAIController::GetBlackBoard() const
{
	return BlackboardComponent;
}

void ABossAIController::Shoot()
{
	FVector ShootDirection = TargetPlayer->GetActorLocation() - GetPawn()->GetActorLocation();
	Cast<ABossTest>(GetPawn())->Shoot(ShootDirection);
}

void ABossAIController::SummonSword()
{
	
	ABossTest* BossTest =  Cast<ABossTest>(GetPawn());
	if(BossTest->CurrentHealth <= 500.0f)
	{
		BossTest->SecondPhaseCallSword();
	}
	else
	{
		BossTest->CallSwordSkill();
	}
}

void ABossAIController::SpawnTree()
{
	ABossTest* BossTest =  Cast<ABossTest>(GetPawn());
	if(BossTest->CurrentHealth <= 500.0f)
	{
		BossTest->TelegraphTreePhase2();
	}
	else
	{
		BossTest->TelegraphTree();
	}
}

void ABossAIController::SpawnBush()
{
	ABossTest* BossTest =  Cast<ABossTest>(GetPawn());
	if(BossTest->CurrentHealth <= 500.0f)
	{
		BossTest->TelegraphBushDelay2Phase();
	}
	else
	{
		BossTest->TelegraphForBush();
	}
}


