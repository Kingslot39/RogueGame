// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_BossJumpAttack.generated.h"

/**
 * 
 */
UCLASS()
class THEGAME_API UBTT_BossJumpAttack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTT_BossJumpAttack();
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName JumpAttack;

	UPROPERTY(EditAnywhere)
	bool bCanAttack;
	

	
};
