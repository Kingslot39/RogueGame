using UnrealBuildTool;

public class Thegame : ModuleRules
{
	public Thegame(ReadOnlyTargetRules Target) : base(Target)	
	{
		PrivateDependencyModuleNames.AddRange(new string[] { "AppFramework", "AppFramework", "AITestSuite" });
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core", "CoreUObject", "Engine", "InputCore", "UMG", "EnhancedInput", 
			"Niagara", "AIModule", "HeadMountedDisplay","GameplayTasks", "NavigationSystem"
			
		});

		// Ensure that no editor-only modules are added
        
		
		
        
	}
}