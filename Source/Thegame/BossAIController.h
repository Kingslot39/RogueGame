// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ThegameCharacter.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BossAIController.generated.h"

/**
 * 
 */
UCLASS()
class THEGAME_API ABossAIController : public AAIController
{
	GENERATED_BODY()
public:
	ABossAIController();

	void BeginPlay() override;
	void OnPossess(APawn* InPawn) override;

	UBlackboardComponent* GetBlackBoard() const;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBlackboardComponent* BlackboardComponent;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UBehaviorTreeComponent* BehaviorTreeComponent;
	
	UPROPERTY(EditAnywhere)
	UBlackboardData* AIBlackboard;
	
	UPROPERTY(EditAnywhere)
	UBehaviorTree* BehaviourTree;

	UPROPERTY(EditAnywhere)
	AThegameCharacter* TargetPlayer;
	
	void Shoot();
	void SummonSword();
	void SpawnTree();
	void SpawnBush();
};
