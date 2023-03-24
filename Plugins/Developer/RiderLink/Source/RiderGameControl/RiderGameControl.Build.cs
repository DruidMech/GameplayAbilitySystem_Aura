// Copyright 1998-2020 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RiderGameControl : ModuleRules
{
	public RiderGameControl(ReadOnlyTargetRules Target) : base(Target)
	{
#if UE_4_22_OR_LATER
		PCHUsage = PCHUsageMode.NoPCHs;
#else
		PCHUsage = PCHUsageMode.NoSharedPCHs;
#endif
		
		bUseRTTI = true;

		PublicDependencyModuleNames.Add("Core");

		PrivateDependencyModuleNames.AddRange(new []
		{
			"RD",
			"RiderLink",
			"HeadMountedDisplay",
			"LevelEditor",
			"UnrealEd",
			"Slate",
			"CoreUObject",
			"Engine"
		});
	}
}
