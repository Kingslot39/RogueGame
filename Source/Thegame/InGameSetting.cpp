#include "InGameSetting.h"

#include "MyGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/PostProcessVolume.h"

void UInGameSetting::NativeConstruct()
{
	Super::NativeConstruct();

	// Get exposure from GameInstance's global value
	UMyGameInstance* GI = GetGameInstance<UMyGameInstance>();
	if (ExposureSlider && GI)
	{
		ExposureSlider->SetValue(GI->GlobalExposure); 
		ApplyExposure(GI->GlobalExposure);
		ExposureSlider->OnValueChanged.AddDynamic(this, &UInGameSetting::OnExposureSliderChanged);
	}
}

void UInGameSetting::OnExposureSliderChanged(float NewValue)
{
	// Update GameInstance and save globally
	UMyGameInstance* GI = GetGameInstance<UMyGameInstance>();
	if (GI)
	{
		GI->GlobalExposure = NewValue;
		GI->SaveGlobalSettings(); // Persist globally
		ApplyExposure(NewValue);
	}
}

void UInGameSetting::SetExposure(float InExposure)
{
	ApplyExposure(InExposure);
}

void UInGameSetting::ApplyExposure(float InExposure)
{
	TArray<AActor*> PostProcessVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APostProcessVolume::StaticClass(), PostProcessVolumes);

	for (AActor* Actor : PostProcessVolumes)
	{
		APostProcessVolume* Volume = Cast<APostProcessVolume>(Actor);
		if (Volume && Volume->bEnabled)
		{
			Volume->Settings.bOverride_AutoExposureBias = true;
			Volume->Settings.AutoExposureBias = InExposure;
		}
	}
}

void UInGameSetting::ApplyExposureToWorld(UWorld* World, float InExposure)
{
	TArray<AActor*> PostProcessVolumes;
	UGameplayStatics::GetAllActorsOfClass(World, APostProcessVolume::StaticClass(), PostProcessVolumes);

	for (AActor* Actor : PostProcessVolumes)
	{
		APostProcessVolume* Volume = Cast<APostProcessVolume>(Actor);
		if (Volume && Volume->bEnabled)
		{
			Volume->Settings.bOverride_AutoExposureBias = true;
			Volume->Settings.AutoExposureBias = InExposure;
		}
	}
}