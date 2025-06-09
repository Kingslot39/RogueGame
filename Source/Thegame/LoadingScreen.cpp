// Fill out your copyright notice in the Description page of Project Settings.

#include "LoadingScreen.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/World.h"

void ULoadingScreen::NativeConstruct()
{
    Super::NativeConstruct();
    
    CurrentProgress = 0.0f;
    TargetProgress = 0.0f;
    ElapsedLoadingTime = 0.0f;
    
    // Initialize progress bar if bound
    if (LoadingProgressBar)
    {
        LoadingProgressBar->SetPercent(0.0f);
    }
    
    UpdateLoadingDisplay();
}

void ULoadingScreen::NativeDestruct()
{
    // Clean up timers
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(LoadingUpdateTimer);
        World->GetTimerManager().ClearTimer(TransitionTimer);
    }
    
    Super::NativeDestruct();
}

void ULoadingScreen::StartLoading(const FString& LevelName)
{
    TargetLevelName = LevelName;
    CurrentProgress = 0.0f;
    TargetProgress = 0.0f;
    ElapsedLoadingTime = 0.0f;
    
    if (UWorld* World = GetWorld())
    {
        // Start updating loading progress
        World->GetTimerManager().SetTimer(
            LoadingUpdateTimer,
            this,
            &ULoadingScreen::SimulateLoadingProgress,
            0.016f, // ~60 FPS update rate
            true
        );
    }
}

void ULoadingScreen::SetLoadingProgress(float Progress)
{
    TargetProgress = FMath::Clamp(Progress, 0.0f, 1.0f);
}

void ULoadingScreen::UpdateLoadingDisplay()
{
    // Update progress bar
    if (LoadingProgressBar)
    {
        LoadingProgressBar->SetPercent(CurrentProgress);
    }
    
    // Update percentage text
    if (PercentageText)
    {
        int32 Percentage = FMath::RoundToInt(CurrentProgress * 100.0f);
        PercentageText->SetText(FText::FromString(FString::Printf(TEXT("%d%%"), Percentage)));
    }
    
    // Update loading text (optional)
    if (LoadingText)
    {
        // You can customize this text or make it cycle through tips
        LoadingText->SetText(FText::FromString(TEXT("Loading...")));
    }
}

void ULoadingScreen::SimulateLoadingProgress()
{
    if (UWorld* World = GetWorld())
    {
        float DeltaTime = World->GetDeltaSeconds();
        ElapsedLoadingTime += DeltaTime;
        
        // Simulate loading progress over time
        if (ElapsedLoadingTime < MinimumLoadingTime)
        {
            // Progress to 95% over the minimum loading time
            TargetProgress = (ElapsedLoadingTime / MinimumLoadingTime) * 0.95f;
        }
        else
        {
            // After minimum time, progress to 100%
            TargetProgress = 1.0f;
        }
        
        // Smoothly animate the progress bar
        if (CurrentProgress < TargetProgress)
        {
            CurrentProgress = FMath::FInterpTo(
                CurrentProgress,
                TargetProgress,
                DeltaTime,
                ProgressAnimationSpeed
            );
            
            UpdateLoadingDisplay();
        }
        
        // Check if we should complete loading
        if (CurrentProgress >= 0.99f && ElapsedLoadingTime >= MinimumLoadingTime)
        {
            // Set to 100% and complete
            CurrentProgress = 1.0f;
            UpdateLoadingDisplay();
            
            // Stop the update timer
            World->GetTimerManager().ClearTimer(LoadingUpdateTimer);
            
            // Small delay before transitioning
            World->GetTimerManager().SetTimer(
                TransitionTimer,
                this,
                &ULoadingScreen::CompleteLoading,
                0.3f,
                false
            );
        }
    }
}

void ULoadingScreen::CompleteLoading()
{
    // Broadcast completion event
    OnLoadingComplete.Broadcast();
    
    // Load the target level
    if (!TargetLevelName.IsEmpty())
    {
        UGameplayStatics::OpenLevel(GetWorld(), *TargetLevelName);
    }
    
    // Remove this widget
    RemoveFromParent();
}