#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RainingArrows.generated.h"

UCLASS()
class THEGAME_API ARainingArrows : public AActor
{
	GENERATED_BODY()
    
public:    
	ARainingArrows();

protected:
	virtual void BeginPlay() override;

	// Spawn a single arrow
	void SpawnArrow();

	// Called when duration expires
	void OnDurationExpired();

	// Properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	float SkillDuration = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	float ArrowSpawnRate = 0.2f; // Spawn arrow every 0.2 seconds

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	float SkillRadius = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	float ArrowDamage = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	TSubclassOf<class ARainArrow> ArrowClass;

private:
	FTimerHandle SpawnTimerHandle;
	FTimerHandle DurationTimerHandle;
	FVector TargetCenter;
};