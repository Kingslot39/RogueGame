#pragma once

#include "CoreMinimal.h"
#include "DialogueStructs.generated.h"

USTRUCT(BlueprintType)
struct FDialogueLine
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	FText Header;
	// The text of the dialogue line
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue", meta = (MultiLine = "true"))
	FText Text;
};