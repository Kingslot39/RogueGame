// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Slider.h"
#include "InGameSetting.generated.h"


UCLASS()
class THEGAME_API UInGameSetting : public UUserWidget
{
	GENERATED_BODY()

public:
	// Bind your slider from the widget blueprint. In your WBP_InGameSetting, add a slider named "ExposureSlider".
	UPROPERTY(meta = (BindWidget))
	USlider* ExposureSlider;
	
	// Call this function when you want to update exposure.
	UFUNCTION(BlueprintCallable)
	void SetExposure(float InExposure);

	// This function applies the given exposure value to all enabled PostProcessVolumes in the provided world.
	UFUNCTION(BlueprintCallable, Category = "Exposure")
	static void ApplyExposureToWorld(UWorld* World, float InExposure);
protected:
	// Called when the widget is constructed.
	virtual void NativeConstruct() override;

private:
	// Applies the exposure value to all enabled post process volumes.
	void ApplyExposure(float InExposure);
	// Handler for slider changes.
	UFUNCTION()
	void OnExposureSliderChanged(float NewValue);
	
};
