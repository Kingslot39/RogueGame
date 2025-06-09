// Fill out your copyright notice in the Description page of Project Settings.


#include "BTD_DistanceToPlayer.h"

#include "AIController.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

UBTD_DistanceToPlayer::UBTD_DistanceToPlayer()
{
	NodeName = TEXT("Distance Check");
}

bool UBTD_DistanceToPlayer::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{ 
	bool bSuccess = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);
	if(!bSuccess)
	{
		return false;
	}
	//get distance between pawn and the player
	float Distance = OwnerComp.GetAIOwner()->GetPawn()->GetDistanceTo(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
	return Distance >= NearDistance && Distance <=FarDistance;
}
