#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BossCombatComponent.generated.h"


USTRUCT(BlueprintType)
struct FAttack
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FString sName;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UAnimMontage* AttackMontage;
	
};

USTRUCT(BlueprintType)
struct FAttackStats
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float Damage;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THEGAME_API UBossCombatComponent : public UActorComponent
{
	GENERATED_BODY()

	
public:	
	// Sets default values for this component's properties
	UBossCombatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TMap<FName, FAttack> AttackMap;

	void Attack(FName AttackName);

	void StopAttack(float BlendedOutTime, FName AttackName);

	bool isAttacking();

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float Health = 100;
	// Stagger montages
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* HurtMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite);
	UAnimMontage* DeadMontage;
	
	//Method for checking if staggered or dying
	//bool IsStaggered();
	//bool IsDying();

	//Damgage Handlers and trace generation handlers
	void GenerateHitSphere(FVector Location, float Radius, FAttackStats AttackStats, bool Debug = false);
	void GenerateHitCapsule(FVector BeginLoc, FVector EndLoc, float fRadius, FAttackStats AttackStats, bool Debug = false);
	void HandleDamage(ACharacter* Victim, FVector Location, FAttackStats Attack);

	TSet<AActor*> DamageActor;

	void TakeDamage(float Damage);

	
	
	

		
};
