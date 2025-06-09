// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "UBuffSelectionWidget.generated.h"
/**
 * 
 */
UCLASS()
class THEGAME_API UUBuffSelectionWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "BuffSelection")
	void InitializeBuffOptions();

	UFUNCTION(BlueprintCallable, Category = "BuffSelection")
	void OnBuffSelected(int32 BuffIndex);
	virtual void NativeConstruct() override;
	UPROPERTY(meta = (BindWidget))
	class UButton* BuffButton1;

	UPROPERTY(meta = (BindWidget))
	class UButton* BuffButton2;

	UPROPERTY(meta = (BindWidget))
	class UButton* BuffButton3;
	// Add references to the TextBlocks that display buff descriptions
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* BuffText1;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* BuffText2;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* BuffText3;
	void BindButtonEvents();
	UFUNCTION()
	void OnBuffButtonClicked1();
	UFUNCTION()
	void OnBuffButtonClicked2();
	UFUNCTION()
	void OnBuffButtonClicked3();
	void ShuffleAndDisplayBuffOptions();
	void UpdateBuffButtonText(UButton* BuffButton, int32 BuffIndex);
	// Add SelectedBuffIndices to store the shuffled buff indices
	TArray<int32> SelectedBuffIndices;
	//ReRollButton
	UPROPERTY(meta = (BindWidget))
	class UButton* RerollButton;
	UFUNCTION()
	void OnRerollClicked();
	void UpdateRerollButtonVisibility();
	UPROPERTY(meta = (BindWidget))
	UImage* BlockingImage;
	UFUNCTION(BlueprintCallable)
	void OnFadeAnimationCompleted();
};
