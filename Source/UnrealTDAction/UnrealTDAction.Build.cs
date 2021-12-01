// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UnrealTDAction : ModuleRules
{
	public UnrealTDAction(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG", "GameplayAbilities"});
		
		PrivateDependencyModuleNames.AddRange(new string[] {  });
		
		PublicIncludePaths.AddRange(new[] { "UnrealTDAction/Public" });

		//The path for the source files
		PrivateIncludePaths.AddRange(new[] { "UnrealTDAction/Private" });
	}
}
