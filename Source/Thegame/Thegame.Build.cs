// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Thegame : ModuleRules
{
	public Thegame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", 
			"Niagara", "AIModule", "HeadMountedDisplay", "GameplayTasks", "UMG"
		});

		// Ensure that no editor-only modules are added
		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.Add("UnrealEd"); // Example, add only if needed for editor builds
		}
	}
}
 