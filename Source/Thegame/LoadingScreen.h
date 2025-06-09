// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "LoadingScreen.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadingComplete);

/**
 * Loading Screen Widget with progress tracking
 */
UCLASS()
class THEGAME_API ULoadingScreen : public UUserWidget
{
    GENERATED_BODY()

public:
    // Initialize the loading screen
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    // Start loading a level with this loading screen
    UFUNCTION(BlueprintCallable, Category = "Loading")
    void StartLoading(const FString& LevelName);

    // Update the loading progress (0.0 to 1.0)
    UFUNCTION(BlueprintCallable, Category = "Loading")
    void SetLoadingProgress(float Progress);

    // Get current loading progress
    UFUNCTION(BlueprintPure, Category = "Loading")
    float GetLoadingProgress() const { return CurrentProgress; }

    // Event called when loading is complete
    UPROPERTY(BlueprintAssignable, Category = "Loading")
    FOnLoadingComplete OnLoadingComplete;

protected:
    // UI Components - Bind these in Blueprint
    UPROPERTY(meta = (BindWidget))
    class UProgressBar* LoadingProgressBar;

    UPROPERTY(meta = (BindWidgetOptional))
    class UTextBlock* LoadingText;

    UPROPERTY(meta = (BindWidgetOptional))
    class UTextBlock* PercentageText;

    // Loading state
    UPROPERTY(BlueprintReadOnly, Category = "Loading")
    float CurrentProgress;

    UPROPERTY(BlueprintReadOnly, Category = "Loading")
    FString TargetLevelName;

    // Timer handles
    FTimerHandle LoadingUpdateTimer;
    FTimerHandle TransitionTimer;

    // Update the visual elements
    UFUNCTION(BlueprintCallable, Category = "Loading")
    void UpdateLoadingDisplay();

    // Internal loading functions
    void SimulateLoadingProgress();
    void CompleteLoading();

    // For smooth progress bar animation
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loading", meta = (ClampMin = "0.0", ClampMax = "10.0"))
    float ProgressAnimationSpeed = 2.0f;

    // Minimum time to show loading screen
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loading", meta = (ClampMin = "0.0"))
    float MinimumLoadingTime = 2.0f;

private:
    float TargetProgress;
    float ElapsedLoadingTime;
};