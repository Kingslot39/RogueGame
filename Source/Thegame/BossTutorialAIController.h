// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BossTutorialAIController.generated.h"

/**
 * 
 */
UCLASS()
class THEGAME_API ABossTutorialAIController : public AAIController
{
	GENERATED_BODY()
public:
	ABossTutorialAIController();

	void BeginPlay() override;
	void OnPossess(APawn* InPawn) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBlackboardComponent* BlackboardComponent;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UBehaviorTreeComponent* BehaviorTreeComponent;
	UPROPERTY(EditAnywhere)
	UBlackboardData* AIBlackboard;
	UPROPERTY(EditAnywhere)
	UBehaviorTree* BehaviourTree;
private:
	
	
};
