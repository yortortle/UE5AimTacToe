// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AimToe : ModuleRules
{
	public AimToe(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
