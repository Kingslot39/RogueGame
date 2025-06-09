#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MouseCursor.generated.h"

class MouseCursor;
UCLASS()
class THEGAME_API AMouseCursorController : public APlayerController
{
	GENERATED_BODY()

public:
	AMouseCursorController();

protected:
	virtual void BeginPlay() override;
	virtual void PlayerTick(float DeltaTime) override;

private:
	void RotateToMouseCursor();
};
