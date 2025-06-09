#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "LevelTrigger.generated.h"

class AThegameCharacter;

UCLASS()
class THEGAME_API ALevelTrigger : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALevelTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame (Only do this on pawn to create accurate movement)
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* LevelTriggerMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* TriggerVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Names")
	TArray<FName> MapName;

	UPROPERTY(EditAnywhere, Category = "Effects")
	UNiagaraSystem* LevelTriggerClass;

private:
	// Store the player controller and character
	APlayerController* MyPlayerController;
	AThegameCharacter* MyCharacter;

	// Camera Locking Variables
	FVector OriginalCameraLocation;
	FRotator OriginalCameraRotation;
	FTimerHandle MovementTimerHandle;
	FTimerHandle FadeTimerHandle;

	UFUNCTION()
	void StartLevelStreaming(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 BodyIndex, bool bSweep, const FHitResult& Hit);

	UFUNCTION()
	void LoadNextLevel();

	UFUNCTION()
	void ContinueForwardMovement();

	UFUNCTION()
	void FadeOutAndLoad();
};
