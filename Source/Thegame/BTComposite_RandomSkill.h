// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "BTComposite_RandomSkill.generated.h"

/**
 * 
 */
UCLASS()
class THEGAME_API UBTComposite_RandomSkill : public UBTCompositeNode
{
	GENERATED_BODY()

	explicit UBTComposite_RandomSkill(const FObjectInitializer& ObjectInitializer);

	virtual int32 GetNextChildHandler(FBehaviorTreeSearchData& SearchData, int32 PrevChild, EBTNodeResult::Type LastResult) const override;
public:
	virtual void InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryInit::Type InitType) const override;
	virtual void CleanupMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryClear::Type CleanupType) const override;
	
};
